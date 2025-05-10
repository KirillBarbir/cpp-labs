#pragma once
#include<filesystem>
#include<fstream>
#include<iostream>
#include<map>
#include<vector>

struct WordInfo {
    uint64_t DID;
    uint64_t line;
};

template<typename T>
T ReadChunkOfData(std::ifstream& fin) {
    char* in = new char[sizeof(T) + 1];
    fin.read(in, sizeof(T));
    T out = *reinterpret_cast<T*>(in);
    delete[] in;
    return out;
}

void VarlenWrite(std::ofstream& fout, uint64_t num);

uint64_t VarlenRead(std::ifstream& fin);

class Trie {
  private:
    struct TrieNode {
        std::vector<WordInfo> from_doc_;
        std::map<char, TrieNode*> children_;
    };

    TrieNode* root;

    void ChainDelete(TrieNode*& cur);

    void ChainCopy(TrieNode*& cur, TrieNode* from);

    void ChainCompress(TrieNode*& cur);
    void ChainDecompress(TrieNode*& cur);

    void ChainWrite(TrieNode* cur, std::ofstream& fout);
    void ChainRead(TrieNode*& cur, std::ifstream& fin);
  public:
    friend class QueryTree;

    Trie() {
        root = new TrieNode;
    }

    ~Trie() {
        ChainDelete(root);
    }
    Trie(Trie& rhs) {
        root = new TrieNode;
        ChainCopy(root, rhs.root);
    }
    Trie& operator=(const Trie& rhs) {
        if (root == rhs.root) {
            return *this;
        }
        ChainDelete(root);
        root = new TrieNode;
        ChainCopy(root, rhs.root);
        return *this;
    }

    void Compress();
    void Decompress();

    void AddWord(std::string_view s, uint64_t DID, uint64_t line);

    void Write(std::ofstream& fout);
    void Read(std::ifstream& fin);

    std::vector<WordInfo>* GetOccurrences(const std::string& term) const;
};

char LowerChar(char x);

class IndexClass {
  public:
    friend class QueryTree;

    IndexClass() = default;
    IndexClass(std::string_view directory) {
        directory_ = directory;
        directory_.push_back('\\');
        for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (dir_entry.is_directory()) {
                continue;
            }
            std::fstream fin(dir_entry.path());
            if (!fin.is_open()) {
                continue;
            }
            std::string x;
            uint64_t cur_dll = 0;
            std::string entry_s = dir_entry.path().string();
            std::string cut_s = entry_s.substr(directory.size() + 1, entry_s.size() - directory.size() - 1);
            DIDs_[cut_s] = N_;

            uint64_t cur_line = 0;
            while (std::getline(fin, x)) {
                std::size_t prev = 0, pos = 0;
                while (pos < x.size()) {
                    std::string to_add;
                    while (isdigit(x[pos]) || ('a' <= LowerChar(x[pos]) && LowerChar(x[pos]) <= 'z')) {
                        to_add.push_back(LowerChar(x[pos]));
                        ++pos;
                    }
                    ++pos;
                    if (to_add.empty()) {
                        continue;
                    }
                    ++cur_dll;
                    data_.AddWord(to_add, DIDs_[cut_s], cur_line);
                }
                ++cur_line;
            }

            dll_sum_ += cur_dll;
            dll_.push_back(cur_dll);
            ++N_;
            fin.close();
        }
    }

    void ReadData(const std::string& directory);
    void WriteData(const std::string& index_name, const std::string& store_directory);
    std::string_view GetFileName(uint64_t to_take);
  private:
    void DeltaCompress();
    void DeltaDecompress();

    Trie data_;
    std::string directory_;
    std::map<std::string, uint64_t> DIDs_;
    std::map<uint64_t, std::string> decode_DIDs_;
    std::vector<uint64_t> dll_;
    uint64_t dll_sum_ = 0;
    uint64_t N_ = 0;
};

