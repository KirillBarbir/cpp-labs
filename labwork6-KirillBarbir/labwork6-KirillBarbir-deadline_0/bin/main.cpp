#include "lib/ArgParser.h"
#include "lib/ArgParserConfig.cpp"
#include "lib/FileManager.h"
#include "lib/HammingCode.h"
#include <bitset>

size_t kMaxFileNameSize = 255;

struct DataChunk {
  uint8_t* data;
  size_t data_size;
};

void BeautifulOutput(uint8_t* data, size_t data_size) { //for debug purposes
    for (size_t i = 0; i < (data_size + 7) / 8; ++i) {
        std::cout << std::bitset<8>(data[i]) << " ";
    }
    std::cout << std::endl;
}

int ErrorOutput(std::string s) {
    std::cerr << s << "\n";
    return 0;
}
/*
 *
 *
 * Архив не имеет информации о себе
 *
 * Каждый файл в архиве имеет (в закодированном виде) следующую информацию:
 * * Размер (размера имени +имени + данных) а битах
 * * * Размер размера в закодированном виде можно выичслить по формуле:
 * * * * В битах: ((8 + ((1 << parity) - parity - 1) - 1) / ((1 << parity) - parity - 1)) * (1 << parity);
 * * * * В байтах:(((8 + ((1 << parity) - parity - 1) - 1) / ((1 << parity) - parity - 1)) * (1 << parity) + 7) / 8;
 * * Размер имени (8 байт)
 * * Имя
 * * Данные
 *
 *
 *
 */
DataChunk ConvertNumberToBytes(size_t data) {
    DataChunk out;
    out.data = new uint8_t[8]{};
    out.data_size = 8;
    size_t byte_mask = (1 << 8) - 1;
    for (int i = 0; i < 8; ++i) {
        out.data[7 - i] = data & byte_mask;
        data >>= 8;
    }
    out.data_size *= 8;
    return out;
}

DataChunk ConvertStringToBytes(std::string_view s) {
    DataChunk out;
    out.data_size = s.size();
    out.data = new uint8_t[out.data_size]{};
    for (int i = 0; i < s.size(); ++i) {
        out.data[i] = s[i];
    }
    out.data_size *= 8;
    return out;
}

size_t ConvertBytesToNumber(uint8_t* data) {
    size_t out = 0;
    for (int i = 0; i < 8; ++i) {
        out += data[i];
        if (i != 7) {
            out <<= 8;
        }
    }
    return out;
}

std::string ConvertBytesToString(uint8_t* data, size_t data_size) {
    data_size = data_size / 8;
    std::string out;
    for (int i = 0; i < data_size; ++i) {
        out.push_back(data[i]);
    }
    return out;
}

void FileAppend(std::string_view file_name, const fs::path& arch_path, size_t parity) {
    size_t file_size = 0;
    fs::path file_path = file_name;
    if (!fs::exists(file_path)) {
        return;
    }

    file_size += fs::file_size(file_path) * 8;
    uint8_t* file_data = GetFileData(file_path, 0, file_size);
    file_data = Encode(file_data, parity, file_size);

    size_t file_data_size = file_size;

    DataChunk name_chunk = ConvertStringToBytes(file_name);
    name_chunk.data = Encode(name_chunk.data, parity, name_chunk.data_size);
    file_size += name_chunk.data_size;

    DataChunk name_chunk_size = ConvertNumberToBytes(name_chunk.data_size);
    name_chunk_size.data = Encode(name_chunk_size.data, parity, name_chunk_size.data_size);

    file_size += name_chunk_size.data_size;

    DataChunk file_size_data = ConvertNumberToBytes(file_size);
    file_size_data.data = Encode(file_size_data.data, parity, file_size_data.data_size);

    FileInsertData(arch_path, file_size_data.data, file_size_data.data_size);
    FileInsertData(arch_path, name_chunk_size.data, name_chunk_size.data_size);
    FileInsertData(arch_path, name_chunk.data, name_chunk.data_size);
    FileInsertData(arch_path, file_data, file_data_size);
    delete[] file_data;
}

uint8_t* GetArchiveData(const fs::path& arch_path, size_t parity, size_t& start_pos, size_t& data_size) {
    uint8_t* data = GetFileData(arch_path, start_pos, data_size);
    start_pos += data_size;
    data = Decode(data, parity, data_size);
    return data;
}

void GoThroughFile(const fs::path& arch_path,
                   size_t parity,
                   size_t start_pos,
                   bool need_to_view_only = false,
                   std::string_view file_name = "",
                   bool need_to_delete = false) {
    size_t absolute_start = start_pos;
    size_t file_size_size = ((64 + ((1 << parity) - parity - 1) - 1) / ((1 << parity) - parity - 1)) * (1 << parity);
    size_t absolute_size = file_size_size;

    uint8_t* data_got_file_size = GetArchiveData(arch_path, parity, start_pos, file_size_size);
    if(data_got_file_size == nullptr){
        return;
    }
    size_t file_size = ConvertBytesToNumber(data_got_file_size);
    absolute_size += file_size;

    size_t name_size_size = ((64 + ((1 << parity) - parity - 1) - 1) / ((1 << parity) - parity - 1)) * (1 << parity);
    uint8_t* data_got_name_size_size = GetArchiveData(arch_path, parity, start_pos, name_size_size);
    if(data_got_name_size_size == nullptr){
        return;
    }
    size_t name_size = ConvertBytesToNumber(data_got_name_size_size);

    uint8_t* raw_name = GetArchiveData(arch_path, parity, start_pos, name_size);
    if(raw_name == nullptr){
        return;
    }
    std::string name = ConvertBytesToString(raw_name, name_size);
    if (need_to_view_only) {
        std::cout << name << "\n";
        if (absolute_size + absolute_start >= fs::file_size(arch_path) * 8) {
            return;
        }
        GoThroughFile(arch_path, parity, absolute_size + absolute_start, true);
    } else { //extract, delete cases
        if (name != file_name) {
            if (absolute_size + absolute_start >= fs::file_size(arch_path) * 8) {
                return;
            }
            GoThroughFile(arch_path, parity, absolute_size + absolute_start, false, file_name, need_to_delete);
            return;
        }
        if (!need_to_delete) {
            size_t raw_data_size = absolute_start + absolute_size - start_pos;
            uint8_t* raw_data = GetArchiveData(arch_path, parity, start_pos, raw_data_size);//
            if(raw_data == nullptr){
                return;
            }
            fs::path file_path(file_name);
            FileInsertData(file_path, raw_data, raw_data_size);
            delete[] raw_data;
        } else {
            FileDeleteData(arch_path, absolute_start, absolute_size);
        }
    }
    delete[] raw_name;
    delete[] data_got_file_size;
    delete[] data_got_name_size_size;
}

void GetFileContents(const fs::path& arch_path, size_t parity, size_t& start_pos) {
    //Extract file size, file name size, file name
    size_t absolute_start = start_pos;
    size_t file_size_size = ((64 + ((1 << parity) - parity - 1) - 1) / ((1 << parity) - parity - 1)) * (1 << parity);
    size_t absolute_size = file_size_size;

    uint8_t* data_got_file_size = GetArchiveData(arch_path, parity, start_pos, file_size_size);
    if(data_got_file_size == nullptr){
        return;
    }
    size_t file_size = ConvertBytesToNumber(data_got_file_size);
    absolute_size += file_size;

    size_t name_size_size = ((64 + ((1 << parity) - parity - 1) - 1) / ((1 << parity) - parity - 1)) * (1 << parity);
    uint8_t* data_got_name_size_size = GetArchiveData(arch_path, parity, start_pos, name_size_size);
    if(data_got_name_size_size == nullptr){
        return;
    }
    size_t name_size = ConvertBytesToNumber(data_got_name_size_size);

    uint8_t* raw_name = GetArchiveData(arch_path, parity, start_pos, name_size);
    if(raw_name == nullptr){
        return;
    }
    std::string name = ConvertBytesToString(raw_name, name_size);

    size_t raw_data_size = absolute_start + absolute_size - start_pos;
    uint8_t* raw_data = GetArchiveData(arch_path, parity, start_pos, raw_data_size);
    if(raw_data == nullptr){
        return;
    }

    fs::path file_path(name);
    FileInsertData(file_path, raw_data, raw_data_size);

    delete[] raw_data;
    delete[] raw_name;
    delete[] data_got_file_size;
    delete[] data_got_name_size_size;
}


int main(int argc, char** argv) {
    ArgumentParser::ArgParser parser = CreateHammingParser();
    if (!parser.Parse(argc, argv)) {
        std::cerr << "Error occurred during parsing\n";
        return 0;
    }
    std::string string_arch_name = parser.GetStringValue("file");
    fs::path arch_path = string_arch_name;
    size_t arch_data_size = 0;
    size_t parity = parser.GetIntValue("parity");
    std::vector<std::string> files = parser.GetStringMultiValue("arguments");
    if (parser.GetFlag("create")) {
        if (fs::exists(arch_path)) {
            return ErrorOutput("This archive is already exist!");
        }
        for (size_t i = 0; i < files.size(); ++i) {
            FileAppend(files[i], arch_path, parity);
        }
    } else if (parser.GetFlag("list")) {
        GoThroughFile(arch_path, parity, 0, true);
    } else if (parser.GetFlag("extract")) {
        if (files.size() == 0) {
            size_t start_pos = 0;
            while (fs::file_size(arch_path) * 8 > start_pos) {
                GetFileContents(arch_path, parity, start_pos);
            }
        }
        for (size_t i = 0; i < files.size(); ++i) {
            GoThroughFile(arch_path, parity, 0, false, files[i], false);
        }
    } else if (parser.GetFlag("append")) {
        for (size_t i = 0; i < files.size(); ++i) {
            FileAppend(files[i], arch_path, parity);
        }
    } else if (parser.GetFlag("delete")) {
        if (files.size() == 0) {
            remove(arch_path);
        }
        for (size_t i = 0; i < files.size(); ++i) {
            GoThroughFile(arch_path, parity, 0, false, files[i], true);
        }
    } else if (parser.GetFlag("concatenate")) {
        for (size_t i = 0; i < files.size(); ++i) {
            fs::path file_path(files[i]);
            uint8_t* file_data = GetFileData(file_path, 0, fs::file_size(file_path) * 8);
            FileInsertData(arch_path, file_data, fs::file_size(file_path) * 8);
            delete[] file_data;
        }
    }
}

