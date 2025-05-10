#include "Searcher.h"

void SkipWhitespaces(const std::string& s, uint64_t& pos, uint64_t end_pos) {
    while (pos < end_pos && s[pos] == ' ') {
        ++pos;
    }
}

uint64_t FindCloseBracket(const std::string& s, uint64_t pos, uint64_t end_pos) {
    ++pos;
    uint64_t cnt = 1;
    while (pos < end_pos && cnt) {
        if (s[pos] == '(') {
            ++cnt;
        }
        if (s[pos] == ')') {
            --cnt;
        }
        ++pos;
    }
    return cnt ? 0 : (pos - 1);
}

void QueryTree::ParseQueryCountAndOutput(IndexClass& index,
                                         const std::string& query,
                                         uint64_t K,
                                         bool output_occurrences,
                                         std::ostream& stream_out) {
    root_ = CreateTree(query, 0, query.size());
    UpdateAllowances(index);
    GenerateTopKHeap(index, K, output_occurrences, stream_out);
}

void QueryTree::ChainClean(QueryTreeNode*& to_be_cleaned) {
    if (to_be_cleaned == nullptr) {
        return;
    }
    ChainClean(to_be_cleaned->left);
    ChainClean(to_be_cleaned->right);
    delete to_be_cleaned;
}

void QueryTree::Clean(std::vector<QueryTreeNode*>& words) {
    for (auto i : words) {
        ChainClean(i);
    }
}

void QueryTree::Merge(QueryTreeNode*& lhs, QueryTreeNode*& rhs, const NodeType& type) {
    auto temp = new QueryTree::QueryTreeNode;
    while (lhs->parent != nullptr) {
        lhs = lhs->parent;
    }
    while (rhs->parent != nullptr) {
        rhs = rhs->parent;
    }
    lhs->parent = temp;
    rhs->parent = temp;
    temp->left = lhs;
    temp->right = rhs;
    temp->query_type = type;
}

void QueryTree::ChainUpdateAllowances(QueryTree::QueryTreeNode*& cur, const IndexClass& index) {
    if (cur == nullptr) {
        return;
    }

    if (cur->left != nullptr) {
        ChainUpdateAllowances(cur->left, index);
    }

    if (cur->right != nullptr) {
        ChainUpdateAllowances(cur->right, index);
    }

    if (cur->query_type == NodeType::kTerm) {
        std::vector<WordInfo>* occ = index.data_.GetOccurrences(cur->term);
        if (occ != nullptr) {
            terms_.push_back(cur->term);
            for (auto i : *occ) {
                cur->allowed.insert(i.DID);
            }
        }
    }

    if (cur->query_type == NodeType::kOr) {
        if (cur->left != nullptr) {
            for (auto i : cur->left->allowed) {
                cur->allowed.insert(i);
            }
            delete cur->left;
            cur->left = nullptr;
        }
        if (cur->right != nullptr) {
            for (auto i : cur->right->allowed) {
                cur->allowed.insert(i);
            }
            delete cur->right;
            cur->right = nullptr;
        }
    }

    if (cur->query_type == NodeType::kAnd) {
        if (cur->left != nullptr) {
            for (auto i : cur->left->allowed) {
                if (cur->right->allowed.count(i)) {
                    cur->allowed.insert(i);
                }
            }
            delete cur->left;
            delete cur->right;
            cur->left = nullptr;
            cur->right = nullptr;
        }
    }
}

void QueryTree::UpdateAllowances(const IndexClass& index) {
    if (root_ == nullptr) {
        return;
    }
    ChainUpdateAllowances(root_, index);
}

void QueryTree::GenerateTopKHeap(IndexClass& index, uint64_t K, bool output_occurrences, std::ostream& stream_out) {
    if (root_ == nullptr) {
        return;
    }
    uint64_t word_count = terms_.size();
    std::vector<std::vector<WordInfo>*> terms_data(word_count);
    std::vector<std::map<uint64_t, uint64_t>> tf(word_count);
    std::vector<uint64_t> df(word_count, 1);
    for (uint64_t i = 0; i < word_count; ++i) {
        terms_data[i] = index.data_.GetOccurrences(terms_[i]);
    }
    for (uint64_t i = 0; i < word_count; ++i) {
        for (uint64_t j = 1; j < (*terms_data[i]).size(); ++j) {
            if ((*terms_data[i])[j].DID != (*terms_data[i])[j - 1].DID) {
                ++df[i];
            }
        }
        for (uint64_t j = 0; j < (*terms_data[i]).size(); ++j) {
            if (root_->allowed.count((*terms_data[i])[j].DID)) {
                if (tf[i].count((*terms_data[i])[j].DID)) {
                    ++tf[i][(*terms_data[i])[j].DID];
                } else {
                    tf[i][(*terms_data[i])[j].DID] = 1;
                }
            }
        }
    }
    for (auto i : root_->allowed) {
        double sum_score = 0;
        for (uint64_t j = 0; j < word_count; ++j) {
            double numerator = static_cast<double>(static_cast<double>(tf[j][i]) * (kKsaturation + 1));

            double b_part = 1.0 - kBsaturation + kBsaturation *
                static_cast<double> (index.dll_[i]) / (static_cast<double>(index.dll_sum_ / index.N_));

            double denumenator = static_cast<double>(static_cast<double>(tf[j][i]) + kKsaturation * b_part);

            double IDF = log2(static_cast<double>(index.N_) / static_cast<double>(df[j]));

            double term_score = (numerator / denumenator) * IDF;

            sum_score += term_score;
        }
        if (top_K_heap_.size() < K) {
            top_K_heap_.push({sum_score, i});
        } else if (top_K_heap_.top().score < sum_score) {
            top_K_heap_.pop();
            top_K_heap_.push({sum_score, i});
        }
    }

    std::stack<DocData> top_K_heap_reversed;
    while (!top_K_heap_.empty()) {
        top_K_heap_reversed.push(top_K_heap_.top());
        top_K_heap_.pop();
    }
    if (top_K_heap_reversed.empty()) {
        stream_out << "No documents found! :(\n" << std::endl;
        delete root_;
        return;
    }
    stream_out << "Most relevant documents:\n";
    uint64_t cur_doc = 1;
    while (!top_K_heap_reversed.empty()) {
        stream_out << cur_doc << ") "
                   << index.directory_ << index.GetFileName(top_K_heap_reversed.top().DID) << "\n";
        if (output_occurrences) {
            stream_out << "Occurrences:\n";
            for (uint64_t i = 0; i < word_count; ++i) {
                int64_t l = -1, r = (*terms_data[i]).size();
                while (r - l > 1) {
                    int64_t m = l + (r - l) / 2;
                    if ((*terms_data[i])[m].DID >= top_K_heap_reversed.top().DID) {
                        r = m;
                    } else {
                        l = m;
                    }
                }
                std::map<uint64_t, uint64_t> line_occ;
                uint64_t total_occurrence = 0;
                for (uint64_t j = r; j < (*terms_data[i]).size()
                    && (*terms_data[i])[j].DID == top_K_heap_reversed.top().DID; ++j) {
                    if (line_occ.count((*terms_data[i])[j].line)) {
                        ++line_occ[(*terms_data[i])[j].line];
                    } else {
                        line_occ[(*terms_data[i])[j].line] = 1;
                    }
                    ++total_occurrence;
                }
                if (line_occ.empty()) {
                    continue;
                }
                stream_out << terms_[i] << " (" << total_occurrence << " in total):\n";
                for (auto j : line_occ) {
                    stream_out << j.second << " in line " << j.first + 1 << '\n';
                }
            }
        }
        top_K_heap_reversed.pop();
        ++cur_doc;
    }
    stream_out << std::endl;
    delete root_;
}

QueryTree::QueryTreeNode* QueryTree::CreateTree(const std::string& query, uint64_t start_pos, uint64_t end_pos) {
    std::size_t pos = start_pos;
    std::vector<QueryTreeNode*> words;
    std::vector<NodeType> operations;
    while (pos < end_pos) {
        SkipWhitespaces(query, pos, end_pos);
        if (pos == end_pos) {
            break;
        }
        if (words.size() == operations.size()) {
            std::string to_add;
            if (query[pos] == '(') {
                auto temp = FindCloseBracket(query, pos, end_pos);
                if (temp == 0) {
                    std::cerr << "Incorrect query(invalid brackets position)\n";
                    Clean(words);
                    return nullptr;
                }
                words.push_back(CreateTree(query, pos + 1, temp));
                if (words[words.size() - 1] == nullptr) {
                    Clean(words);
                    return nullptr;
                }
                pos = temp;
                ++pos;
            }
            while (isdigit(query[pos]) || ('a' <= LowerChar(query[pos]) && LowerChar(query[pos]) <= 'z')) {
                to_add.push_back(LowerChar(query[pos]));
                ++pos;
            }
            ++pos;
            if (to_add.empty()) {
                continue;
            }
            QueryTreeNode* temp = new QueryTreeNode;
            temp->term = to_add;
            words.push_back(temp);
        } else {
            auto and_pos = query.find("AND", pos);
            auto or_pos = query.find("OR", pos);
            if (and_pos == pos) {
                operations.push_back(NodeType::kAnd);
                pos += 3;
            } else if (or_pos == pos) {
                operations.push_back(NodeType::kOr);
                pos += 2;
            } else {
                std::cerr << "Incorrect query(AND or OR was not found)\n";
                Clean(words);
                return nullptr;
            }
        }

    }
    if (words.size() != operations.size() + 1) {
        std::cerr << "Invalid query(words and operations mismatch)\n";
        Clean(words);
        return nullptr;
    }
    uint64_t operations_size = operations.size();
    for (uint64_t i = 0; i < operations_size; ++i) {
        if (operations[i] == NodeType::kAnd) {
            Merge(words[i], words[i + 1], NodeType::kAnd);
        }
    }
    for (uint64_t i = 0; i < operations_size; ++i) {
        if (operations[i] == NodeType::kOr) {
            Merge(words[i], words[i + 1], NodeType::kOr);
        }
    }
    while (words[0] != nullptr && words[0]->parent != nullptr) {
        words[0] = words[0]->parent;
    }
    return words[0];
}
