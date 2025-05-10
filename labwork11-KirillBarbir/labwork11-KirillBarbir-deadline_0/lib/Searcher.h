#pragma once
#include<cmath>
#include<list>
#include<queue>
#include<stack>
#include<set>
#include "Indexer.h"

enum class NodeType {
    kAnd,
    kOr,
    kTerm
};

const double kKsaturation = 2.0;
const double kBsaturation = 0.75;

void SkipWhitespaces(const std::string& s, uint64_t& pos, uint64_t end_pos);

uint64_t FindCloseBracket(const std::string& s, uint64_t pos, uint64_t end_pos);

struct DocData {
    double score = 0;
    uint64_t DID;
};
struct cmp {
    bool operator()(const DocData& lhs, const DocData& rhs) {
        return lhs.score > rhs.score;
    }
};

class QueryTree {
  public:
    friend IndexClass;
    struct QueryTreeNode {
        std::string term;
        std::set<uint64_t> allowed;
        NodeType query_type = NodeType::kTerm;
        QueryTreeNode* left = nullptr;
        QueryTreeNode* right = nullptr;
        QueryTreeNode* parent = nullptr;
    };

    QueryTree() = default;

    void ParseQueryCountAndOutput(IndexClass& index,
                                  const std::string& query,
                                  uint64_t K = 100,
                                  bool output_occurrences = false,
                                  std::ostream& stream_out = std::cout);

  private:
    void ChainClean(QueryTreeNode*& to_be_cleaned);
    void Clean(std::vector<QueryTreeNode*>& words);
    void Merge(QueryTree::QueryTreeNode*& lhs, QueryTree::QueryTreeNode*& rhs, const NodeType& type);
    void ChainUpdateAllowances(QueryTreeNode*& cur, const IndexClass& index);
    void UpdateAllowances(const IndexClass& index);
    void GenerateTopKHeap(IndexClass& index, uint64_t K, bool output_occurrences, std::ostream& stream_out);
    QueryTreeNode* CreateTree(const std::string& query, uint64_t start_pos, uint64_t end_pos);

    QueryTreeNode* root_;
    std::vector<std::string> terms_;
    std::priority_queue<DocData, std::vector<DocData>, cmp> top_K_heap_;
};



