#include "FileManager.h"

uint8_t* GetFileData(const std::filesystem::path& file_name, size_t start_pos, size_t amount) {
    start_pos = (start_pos + 7) / 8;
    amount = (amount + 7) / 8;
    uint8_t* s = new uint8_t[amount];
    std::ifstream input_stream(file_name, std::ios_base::binary);
    if (!input_stream.is_open()) {
        return nullptr;
    }
    input_stream.seekg(start_pos);
    input_stream.read(reinterpret_cast<char*>(s), amount);
    return s;
}

void FileInsertData(const std::filesystem::path& file_name, uint8_t*& data, size_t data_size) {
    data_size = (data_size + 7) / 8;
    std::ofstream output_stream(file_name, std::ios_base::binary | std::ios::app);
    if (!output_stream.is_open()) {
        return;
    }
    output_stream.write(reinterpret_cast<char*>(data), data_size);
}

void FileDeleteData(const std::filesystem::path& file_name, size_t absolute_start, size_t absolute_size) {
    uint8_t* first_part = GetFileData(file_name, 0, absolute_start);
    size_t sec_start = absolute_start + absolute_size;
    uint8_t* second_part = GetFileData(file_name, sec_start, fs::file_size(file_name) * 8 - sec_start);
    size_t sec_part_size = fs::file_size(file_name) * 8 - sec_start;
    remove(file_name);
    FileInsertData(file_name, first_part, absolute_start);
    FileInsertData(file_name, second_part, sec_part_size);
    delete[] first_part;
    delete[] second_part;
}