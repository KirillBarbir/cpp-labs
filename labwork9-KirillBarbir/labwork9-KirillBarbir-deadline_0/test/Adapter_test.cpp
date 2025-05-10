#include <gtest/gtest.h>
#include "lib/Adapter.h"

TEST(Adaptertests, TransformTest1) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> ans = {4, 9, 16, 25, 36, 49};
    std::function func = [](int i) { return i + 1; };
    auto result = v | transform(func) | transform([](int i) { return i * i; });
    int j = 0;
    for (auto i : result) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, TransformTest2) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> ans = {4, 9, 16, 25, 36, 49};
    std::function func = [](int i) { return i + 1; };
    auto result = transform(transform(v, func), [](int i) { return i * i; });
    int j = 0;
    for (auto i : result) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, FilterTest) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> ans = {2, 4, 6, 8};
    std::function func = [](int i) { return i % 2 == 1; };
    auto result = filter((v | filter(func)) | filter(func), [](int i) { return i > 10; });
    int j = 0;
    for (auto i : result) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, FilterTransformTest) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> ans = {1, 9, 25};
    int j = 0;
    for (int i : v | filter([](int i) { return i % 2; }) | transform([](int i) { return i * i; })) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, DropTest) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> ans = {6, 7, 8, 9, 10};
    std::function func = [](int i) { return i % 2 == 1; };
    auto result = drop(drop(v, 1), 1);
    int j = 0;
    for (auto i : result | drop(1) | drop(1) | drop(1)) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, TakeTest) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> ans = {1, 2, 3, 4, 5};
    std::function func = [](int i) { return i % 2 == 1; };
    auto result = take(take(v, 9), 8);
    int j = 0;
    for (auto i : result | take(7) | take(6) | take(5)) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}


TEST(Adaptertests, ReverseTest) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> ans = {6, 5, 4, 3, 2, 1};
    std::function func = [](int i) { return i % 2 == 1; };
    auto result = reverse(reverse(v));
    int j = 0;
    for (auto i : result | reverse | reverse | reverse) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, CombinedTest) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> ans = {25, 9};
    std::function func = [](int i) { return i % 2;};
    int j = 0;
    for (auto i : v | drop(1) | reverse | take(5) | drop(1) | reverse | reverse | filter(func) |
    transform([](int i){return i * i;})) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, EfficencyTimeStamp) {
    std::vector<int> v(1e7, 0);
    std::vector<int> ans (1e7, 0);
    std::function func = [](int i) { return i % 2;};
    int j = 0;
    for (auto i : v) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, EfficencyTestMultipleOperations) {
    std::vector<int> v(1e7, 0);
    std::vector<int> ans (1e7, 0);
    std::function func = [](int i) { return i % 2;};
    int j = 0;
    for (auto i : v | reverse | reverse | reverse | filter(func) | transform(func) |
        transform([](int i){return i * i;})) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, KeysTest1) {
    std::map<int, int> m;
    std::vector<int> ans = {1, 3, 5};
    m[1] = 2;
    m[3] = 4;
    m[5] = 6;
    int j = 0;
    for (auto i : m | keys) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, KeysTest2) {
    std::map<int, int> m;
    std::vector<int> ans = {5, 3, 1};
    m[1] = 2;
    m[3] = 4;
    m[5] = 6;
    int j = 0;
    for (auto i : m | reverse | keys) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, ValuesTest1) {
    std::map<int, int> m;
    std::vector<int> ans = {2, 4, 6};
    m[1] = 2;
    m[3] = 4;
    m[5] = 6;
    int j = 0;
    for (auto i : m | values) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, ValuesTest2) {
    std::map<int, int> m;
    std::vector<int> ans = {6, 4, 2};
    m[1] = 2;
    m[3] = 4;
    m[5] = 6;
    int j = 0;
    for (auto i : m | reverse | values) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, ValuesTest3) {
    std::map<int, char> m;
    std::vector<int> ans = {'6', '4', '2'};
    m[1] = '2';
    m[3] = '4';
    m[5] = '6';
    int j = 0;
    for (auto i : m | reverse | values) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}

TEST(Adaptertests, CombinedTest2) {
    std::map<int, int> v;
    v[1] = 11;
    v[2] = 12;
    v[3] = 13;
    v[4] = 14;
    v[5] = 15;
    v[6] = 16;
    std::vector<int> ans = {25, 9};
    std::function func = [](int i) { return i % 2;};
    int j = 0;
    for (auto i : v | drop(1) | reverse | take(5) | drop(1) | reverse | reverse | keys | filter(func) |
        transform([](int i){return i * i;})) {
        ASSERT_EQ(i, ans[j]);
        ++j;
    }
}