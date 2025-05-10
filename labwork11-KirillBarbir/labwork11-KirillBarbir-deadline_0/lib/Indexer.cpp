#include "Indexer.h"

void VarlenWrite(std::ofstream& fout, uint64_t num) {
    while (1) {
        uint8_t x = num % (1 << 7);
        num >>= 7;
        if (!num) {
            x += (1 << 7);
            fout.write(reinterpret_cast<char*>(&x), 1);
            break;
        }
        fout.write(reinterpret_cast<char*>(&x), 1);
    }
}

uint64_t VarlenRead(std::ifstream& fin) {
    uint8_t x;
    uint64_t it = 0;
    uint64_t out = 0;
    while (1) {
        x = ReadChunkOfData<uint8_t>(fin);
        if (x >= 128) {
            x -= 128;
            out += x << (it * 7);
            break;
        }
        out += x << (it * 7);
        ++it;

    }
    return out;
}

//TRIE methods ************************************************************************
void Trie::ChainDelete(Trie::TrieNode*& cur) {
    for (auto x : cur->children_) {
        ChainDelete(x.second);
    }
    delete cur;
}

void Trie::ChainCopy(Trie::TrieNode*& cur, Trie::TrieNode* from) {
    //cur->value_ = from->value_;
    for (auto x : from->children_) {
        cur->children_[x.first] = new TrieNode;
        ChainCopy(cur->children_[x.first], x.second);
    }
}

void Trie::ChainCompress(Trie::TrieNode*& cur) {
    TrieNode* temp = cur;
    for (int64_t i = temp->from_doc_.size() - 1; i > 0; --i) {
        temp->from_doc_[i].DID -= temp->from_doc_[i - 1].DID;
    }
    for (auto x : cur->children_) {
        ChainCompress(x.second);
    }
}

void Trie::ChainDecompress(Trie::TrieNode*& cur) {
    TrieNode* temp = cur;
    for (int64_t i = 1; i < temp->from_doc_.size(); ++i) {
        temp->from_doc_[i].DID += temp->from_doc_[i - 1].DID;
    }
    for (auto x : cur->children_) {
        ChainDecompress(x.second);
    }
}

void Trie::ChainWrite(Trie::TrieNode* cur, std::ofstream& fout) {
    uint64_t children_size = cur->children_.size();
    fout.write(reinterpret_cast<char*>(&children_size), sizeof(uint64_t));//8
    for (auto i : cur->children_) {
        fout.write(&i.first, 1); // csize
        uint64_t doc_amount = i.second->from_doc_.size();
        fout.write(reinterpret_cast<char*>(&doc_amount), sizeof(uint64_t));//8 * csize
        for (auto j : i.second->from_doc_) {//idk
            VarlenWrite(fout, j.DID);
            VarlenWrite(fout, j.line);
        }
    }
    for (auto i : cur->children_) {
        ChainWrite(i.second, fout);
    }
}

void Trie::ChainRead(Trie::TrieNode*& cur, std::ifstream& fin) {
    auto children_size = ReadChunkOfData<uint64_t>(fin);
    for (uint64_t i = 0; i < children_size; ++i) {
        char c = ReadChunkOfData<char>(fin);
        cur->children_[c] = new TrieNode;
        auto doc_amount = ReadChunkOfData<uint64_t>(fin);
        for (uint64_t j = 0; j < doc_amount; ++j) {
            auto DID = VarlenRead(fin);
            uint64_t line = 0;
            line = VarlenRead(fin);
            cur->children_[c]->from_doc_.push_back({DID, line});
        }
    }
    for (auto i : cur->children_) {
        ChainRead(i.second, fin);
    }
}

void Trie::AddWord(std::string_view s, uint64_t DID, uint64_t line) {
    TrieNode* temp = root;
    for (auto i : s) {
        if (!temp->children_.count(i)) {
            temp->children_[i] = new TrieNode;
        }
        temp = temp->children_[i];
    }
    temp->from_doc_.push_back({DID, line});
}

void Trie::Write(std::ofstream& fout) {
    ChainWrite(root, fout);
}

void Trie::Read(std::ifstream& fin) {
    ChainRead(root, fin);
}

void Trie::Compress() {
    ChainCompress(root);
}

void Trie::Decompress() {
    ChainDecompress(root);
}

std::vector<WordInfo>* Trie::GetOccurrences(const std::string& term) const {
    auto temp = root;
    for (auto i : term) {
        if (!temp->children_.count(i)) {
            return nullptr;
        }
        temp = temp->children_[i];
    }
    return &temp->from_doc_;
}

//*************************************************************************************8
char LowerChar(char x) {
    if ('A' <= x && x <= 'Z') {
        return static_cast<char>(x - 'A' + 'a');
    }
    return x;
}

void IndexClass::DeltaCompress() {
    data_.Compress();
}

void IndexClass::DeltaDecompress() {
    data_.Decompress();
}

void IndexClass::ReadData(const std::string& path) {
    std::ifstream fin(path, std::ios::binary);
    if (!fin.is_open()) {
        std::cerr << "Unable to open file";
    }
    N_ = ReadChunkOfData<uint64_t>(fin);
    dll_sum_ = ReadChunkOfData<uint64_t>(fin);
    auto dir_size = ReadChunkOfData<uint64_t>(fin);
    for (uint64_t i = 0; i < dir_size; ++i) {
        directory_.push_back(ReadChunkOfData<char>(fin));
    }
    for (uint64_t i = 0; i < N_; ++i) {
        dll_.push_back(ReadChunkOfData<uint64_t>(fin));
    }
    for (uint64_t i = 0; i < N_; ++i) {
        auto cur_size = ReadChunkOfData<uint64_t>(fin);
        std::string s;
        for (uint64_t j = 0; j < cur_size; ++j) {
            auto c = ReadChunkOfData<char>(fin);
            s.push_back(c);
        }
        auto DID = ReadChunkOfData<uint64_t>(fin);
        decode_DIDs_[DID] = s;
    }
    data_.Read(fin);
    fin.close();
    DeltaDecompress();
}

void IndexClass::WriteData(const std::string& index_name, const std::string& store_directory) {
    std::ofstream fout(store_directory + "\\" + index_name, std::ios::binary);
    if (!fout.is_open()) {
        std::cerr << "Unable to create index file";
        return;
    }
    DeltaCompress();
    uint64_t dir_size = directory_.size();
    fout.write(reinterpret_cast<char*>(&N_), sizeof(uint64_t));
    fout.write(reinterpret_cast<char*>(&dll_sum_), sizeof(uint64_t));
    fout.write(reinterpret_cast<char*>(&dir_size), sizeof(uint64_t));
    for (char i : directory_) {
        fout.write(&i, 1);
    }
    for (uint64_t i : dll_) {
        fout.write(reinterpret_cast<char*>(&i), sizeof(uint64_t));
    }
    for (auto i : DIDs_) {
        uint64_t cur_size = i.first.size();
        fout.write(reinterpret_cast<char*>(&cur_size), sizeof(uint64_t));
        for (uint64_t j = 0; j < cur_size; ++j) {
            fout.write(&i.first[j], sizeof(char));
        }
        fout.write(reinterpret_cast<char*>(&i.second), sizeof(uint64_t));

    }
    data_.Write(fout);
    fout.close();
}

std::string_view IndexClass::GetFileName(uint64_t to_take) {
    return decode_DIDs_[to_take];
}