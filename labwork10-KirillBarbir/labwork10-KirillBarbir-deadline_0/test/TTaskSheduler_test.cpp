#include <gtest/gtest.h>
#include <cmath>
#include "lib/TTaskSheduler.h"

using namespace Scheduler;

TEST(TTaskSheduler_tests, single_sum_test1) {
    TTaskSheduler sheduler;
    auto out = sheduler.add([](int a, int b) { return a + b; }, 10, 15);
    ASSERT_EQ(25, sheduler.getResult<int>(out));
}

class SumFunctor {
  public:
    int operator()(int a, int b) const {
        return a + b;
    }
};

TEST(TTaskSheduler_tests, single_sum_test2) {
    TTaskSheduler sheduler;
    SumFunctor summ;
    auto out = sheduler.add(summ, 10, 15);
    ASSERT_EQ(25, sheduler.getResult<int>(out));
}

int summ(int a, int b) {
    return a + b;
}

TEST(TTaskSheduler_tests, single_sum_test3) {
    TTaskSheduler sheduler;
    auto out = sheduler.add(&summ, 10, 15);
    ASSERT_EQ(25, sheduler.getResult<int>(out));
}

TEST(TTaskSheduler_tests, single_container_test1) {
    TTaskSheduler sheduler;
    std::vector<char> in = {'a', 'b', 'c', 'd'};
    auto out = sheduler.add([](std::vector<char> a, int b) { return a[b]; }, in, 1);
    ASSERT_EQ('b', sheduler.getResult<char>(out));
}

class Ind {
  public:
    char operator()(std::vector<char> a, int b) const {
        return a[b];
    }
};

TEST(TTaskSheduler_tests, single_container_test2) {
    TTaskSheduler sheduler;
    Ind ind;
    std::vector<char> in = {'a', 'b', 'c', 'd'};
    auto out = sheduler.add(ind, in, 1);
    ASSERT_EQ('b', sheduler.getResult<char>(out));
}

TEST(TTaskSheduler_tests, single_execute_sum_test) {
    TTaskSheduler sheduler;
    auto out = sheduler.add([](int a, int b) { return a + b; }, 10, 15);
    sheduler.executeAll();
    ASSERT_EQ(25, sheduler.getResult<int>(out));
}

TEST(TTaskSheduler_tests, single_execute_container_test) {
    TTaskSheduler sheduler;
    std::vector<char> in = {'a', 'b', 'c', 'd'};
    auto out = sheduler.add([](std::vector<char> a, int b) { return a[b]; }, in, 1);
    sheduler.executeAll();
    ASSERT_EQ('b', sheduler.getResult<char>(out));
}

TEST(TTaskSheduler_tests, multiple_operations_test1) {
    TTaskSheduler sheduler;
    auto ID1 = sheduler.add([](int a, int b) { return a + b; }, 10, 15);
    auto ID2 = sheduler.add([](int a, int b) { return a + b; }, 100, 150);
    auto ID3 = sheduler.add([](int a, int b) { return a + b; },
                            sheduler.getFutureResult<int>(ID1), sheduler.getFutureResult<int>(ID2));
    ASSERT_EQ(275, sheduler.getResult<int>(ID3));
}

TEST(TTaskSheduler_tests, multiple_operations_test2) {
    TTaskSheduler sheduler;
    auto ID1 = sheduler.add([](int a, int b) { return a + b; }, 10, 15);
    auto ID2 = sheduler.add([](int a, int b) { return a + b; },
                            sheduler.getFutureResult<int>(ID1), 150);
    auto ID3 = sheduler.add([](int a, int b) { return a + b; },
                            100, sheduler.getFutureResult<int>(ID2));
    ASSERT_EQ(275, sheduler.getResult<int>(ID3));
}

TEST(TTaskSheduler_tests, multiple_operations_test3) {
    TTaskSheduler sheduler;
    auto ID1 = sheduler.add([](int a, int b) { return a + b; }, 1, 1);
    auto ID2 = sheduler.add([](int a, int b) { return a + b; },
                            sheduler.getFutureResult<int>(ID1), sheduler.getFutureResult<int>(ID1));
    auto ID3 = sheduler.add([](int a, int b) { return a + b; },
                            sheduler.getFutureResult<int>(ID2), sheduler.getFutureResult<int>(ID2));
    auto ID4 = sheduler.add([](int a, int b) { return a + b; },
                            sheduler.getFutureResult<int>(ID3), sheduler.getFutureResult<int>(ID3));
    ASSERT_EQ(16, sheduler.getResult<int>(ID4));
}

TEST(TTaskSheduler_tests, multiple_operations_test4) {
    TTaskSheduler sheduler;
    std::vector<char> in = {'a', 'b', 'c', 'd'};
    auto ID1 = sheduler.add([](std::vector<char> a, int b) { return a[b]; }, in, 1);
    auto ID2 = sheduler.add([](char a, std::vector<char> b) { return b[a - 'a']; },
                            sheduler.getFutureResult<char>(ID1), in);
    ASSERT_EQ('b', sheduler.getResult<char>(ID2));
}

TEST(TTaskSheduler_tests, other_arg_count_test1) {
    TTaskSheduler sheduler;
    auto ID1 = sheduler.add([](int a) { return a * a; }, 10);
    ASSERT_EQ(100, sheduler.getResult<int>(ID1));
}

TEST(TTaskSheduler_tests, other_arg_count_test2) {
    TTaskSheduler sheduler;
    auto ID1 = sheduler.add([](int a) { return a * a; }, 10);
    auto ID2 = sheduler.add([](int a) { return a * a; },
                            sheduler.getFutureResult<int>(ID1));
    ASSERT_EQ(10000, sheduler.getResult<int>(ID2));
}

TEST(TTaskSheduler_tests, other_arg_count_test3) {
    TTaskSheduler sheduler;
    auto ID1 = sheduler.add([]() { return "HI!"; });

    ASSERT_EQ("HI!", sheduler.getResult<const char*>(ID1));
}

TEST(TTaskSheduler_tests, sample_test) {
    float a = 1;
    float b = -2;
    float c = 0;

    TTaskSheduler scheduler;

    auto id1 = scheduler.add([](float a, float c) { return -4 * a * c; }, a, c);

    auto id2 = scheduler.add([](float b, float v) { return b * b + v; }, b, scheduler.getFutureResult<float>(id1));

    auto id3 =
        scheduler.add([](float b, float d) { return -b + std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));

    auto id4 =
        scheduler.add([](float b, float d) { return -b - std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));

    auto id5 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id3));

    auto id6 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id4));

    scheduler.executeAll();
    ASSERT_EQ(2, scheduler.getResult<float>(id5));
    ASSERT_EQ(0, scheduler.getResult<float>(id6));
}
