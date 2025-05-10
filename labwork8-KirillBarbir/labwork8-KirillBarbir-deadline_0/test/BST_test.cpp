#include <gtest/gtest.h>
#include "lib/BST.h"

TEST(BSTtests, construction_test1) {
    BST<int> bst;
    ASSERT_TRUE(bst.empty());
}

TEST(BSTtests, construction_test2) {
    BST<int> bst;
    BST<int> bst2 = bst;
    ASSERT_TRUE(bst2.empty());
}

TEST(BSTtests, construction_test3) {
    BST<int> bst;
    BST<int> bst2(bst);
    ASSERT_TRUE(bst2.empty());
}

TEST(BSTtests, construction_test4) {
    BST<int> bst{1, 2, 3, 4};
    BST<int> bst2(bst);
    ASSERT_EQ(bst2.size(), 4);
}

TEST(BSTtests, equality_test1) {
    BST<int> bst{1, 2, 3, 4};
    BST<int> bst2;
    bst2.insert({1, 2, 3, 4});
    ASSERT_TRUE(bst == bst2);
}

TEST(BSTtests, equality_test2) {
    BST<int> bst{1, 2, 3, 4};
    BST<int> bst2;
    bst2.insert(1);
    bst2.insert(2);
    bst2.insert(3);
    bst2.insert(4);
    ASSERT_TRUE(bst == bst2);
}

TEST(BSTtests, equality_test3) {
    BST<int> bst{1, 2, 3, 4};
    BST<int> bst2 = bst;
    ASSERT_TRUE(bst == bst2);
}

TEST(BSTtests, equality_test4) {
    BST<int> bst{1, 2, 3, 4};
    BST<int> bst2(bst);
    ASSERT_TRUE(bst == bst2);
}

TEST(BSTtests, equality_test5) {
    BST<int> bst{1, 2, 3, 4};
    BST<int> bst2{2, 1, 3, 4};
    ASSERT_TRUE(bst != bst2);
}

TEST(BSTtests, equality_test6) {
    BST<int> bst{1, 2, 3, 4};
    BST<int> bst2(bst.begin<inorder_tag>(), bst.end<inorder_tag>());
    ASSERT_TRUE(bst == bst2);
}

TEST(BSTtests, inorder_iterator_test1) {
    BST<int> bst{9, 5, 11, 10, 13, 14, 8, 7, 6, 2, 1, 3, 4};
    ASSERT_TRUE(*bst.begin<inorder_tag>() == 1);
}

TEST(BSTtests, inorder_iterator_test2) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    BST<int>::iterator<inorder_tag> it1 = bst.begin();
    int a[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    for (int i = 0; i < 13; ++i) {
        ASSERT_EQ(*it1, a[i]);
        ++it1;
    }
}

TEST(BSTtests, inorder_iterator_test3) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    BST<int>::iterator<inorder_tag> it1 = bst.begin();
    int a[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    for (int i = 0; i < 13; ++i) {
        ASSERT_EQ(*it1++, a[i]);
    }
}

TEST(BSTtests, inorder_iterator_test4) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    BST<int>::iterator<inorder_tag> it1 = bst.end();
    int a[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    for (int i = 12; i >= 0; --i) {
        --it1;
        ASSERT_EQ(*it1, a[i]);
    }
}

TEST(BSTtests, preorder_iterator_test1) {
    BST<int> bst{9, 5, 11, 10, 13, 14, 8, 7, 6, 2, 1, 3, 4};
    ASSERT_TRUE(*bst.begin<preorder_tag>() == 9);
}

TEST(BSTtests, preorder_iterator_test2) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    BST<int>::iterator<preorder_tag> it1 = bst.begin<preorder_tag>();
    int a[13] = {9, 5, 2, 1, 3, 4, 8, 7, 6, 11, 10, 12, 13};
    for (int i = 0; i < 13; ++i) {
        ASSERT_EQ(*it1, a[i]);
        ++it1;
    }
}

TEST(BSTtests, preorder_iterator_test3) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    BST<int>::iterator<preorder_tag> it1 = bst.end<preorder_tag>();
    int a[13] = {9, 5, 2, 1, 3, 4, 8, 7, 6, 11, 10, 12, 13};
    for (int i = 12; i >= 0; --i) {
        --it1;
        ASSERT_EQ(*it1, a[i]);
    }
}

TEST(BSTtests, postorder_iterator_test1) {
    BST<int> bst{9, 5, 11, 10, 13, 14, 8, 7, 6, 2, 1, 3, 4};
    ASSERT_TRUE(*bst.begin<preorder_tag>() == 9);
}

TEST(BSTtests, postorder_iterator_test2) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    BST<int>::iterator<postorder_tag> it1 = bst.begin<postorder_tag>();
    int a[13] = {1, 4, 3, 2, 6, 7, 8, 5, 10, 13, 12, 11, 9};
    for (int i = 0; i < 13; ++i) {
        ASSERT_EQ(*it1, a[i]);
        ++it1;
    }
}

TEST(BSTtests, postorder_iterator_test3) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    BST<int>::iterator<postorder_tag> it1 = bst.end<postorder_tag>();
    int a[13] = {1, 4, 3, 2, 6, 7, 8, 5, 10, 13, 12, 11, 9};
    for (int i = 12; i >= 0; --i) {
        --it1;
        ASSERT_EQ(*it1, a[i]);
    }
}

TEST(BSTtests, erase_test) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    int k = 13;
    bst.erase(k);
    BST<int>::iterator<inorder_tag> it1 = bst.begin();
    int a[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    for (int i = 0; i < 12; ++i) {
        ASSERT_EQ(*it1, a[i]);
        ++it1;
    }
}

TEST(BSTtests, insert_test) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    bst.insert({14, 18, 16});
    BST<int>::iterator<inorder_tag> it1 = bst.begin();
    int a[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 18};
    for (int i = 0; i < 16; ++i) {
        ASSERT_EQ(*it1, a[i]);
        ++it1;
    }
}

TEST(BSTtests, reverse_iterator_test) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    BST<int>::reverse_iterator<inorder_tag> it1 = bst.rbegin<inorder_tag>();
    int a[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    for (int i = 12; i >= 0; --i) {
        ASSERT_EQ(*it1, a[i]);
        ++it1;
    }
}

TEST(BSTtests, clear_test) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    bst.clear();
    ASSERT_TRUE(bst.empty());
}

TEST(BSTtests, count_test) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    ASSERT_TRUE(bst.count(3));
}

TEST(BSTtests, find_test) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    bool res = (bst.begin() == bst.find(1));
    ASSERT_TRUE(res);
}

TEST(BSTtests, contains_test1) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    ASSERT_TRUE(bst.contains(11));
}

TEST(BSTtests, contains_test2) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    ASSERT_FALSE(bst.contains(14));
}

TEST(BSTtests, upper_bound_test) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    int k = *(bst.upper_bound(0));
    ASSERT_EQ(k, 1);
}

TEST(BSTtests, lower_bound_test) {
    BST<int> bst{9, 5, 11, 10, 12, 13, 8, 7, 6, 2, 1, 3, 4};
    ASSERT_EQ(*(bst.lower_bound(14)), 13);
}
