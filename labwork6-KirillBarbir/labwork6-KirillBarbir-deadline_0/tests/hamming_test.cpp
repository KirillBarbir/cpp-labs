#include <gtest/gtest.h>
#include "lib/HammingCode.h"
#include <bitset>
TEST(HammingTests, SelfAssignment1){
    uint8_t* s1 = new uint8_t[1]{};
    uint8_t* s2 = new uint8_t[1]{};
    size_t data_size = 8;
    s1 = Encode(s1, 3, data_size);
    s1 = Decode(s1, 3, data_size);
    ASSERT_EQ(*s1, *s2);
}

TEST(HammingTests, SelfAssignment2){
    uint8_t* s1 = new uint8_t[3]{};
    uint8_t* s2 = new uint8_t[3]{};
    for(int i = 0; i < 3; ++i){
        s1[i] = i + 1;
        s2[i] = i + 1;
    }
    size_t data_size = 24;
    s1 = Encode(s1, 3, data_size);
    s1 = Decode(s1, 3, data_size);
    for(int i = 0; i < 3; ++i) {
        ASSERT_EQ(s1[i], s2[i]);
    }
}

TEST(HammingTests, EasyTest){
    uint8_t* s1 = new uint8_t[2]{};
    uint8_t* s4 = new uint8_t[2]{};
    s1[0] = 0;
    s1[1] = 0;
    size_t data_size = 8;
    uint8_t* s2 = Encode(s1, 3, data_size);
    uint8_t* s3 = Decode(s2, 3, data_size);
    ASSERT_EQ(*s4, *s3);
}

TEST(HammingTests, SingleDigitTest){
    uint8_t* s1 = new uint8_t[1]{};
    uint8_t* s4 = new uint8_t[1]{};
    s1[0] = 128;
    s4[0] = 128;
    size_t data_size = 8;
    uint8_t* s2 = Encode(s1, 3, data_size);
    uint8_t* s3 = Decode(s2, 3, data_size);
    ASSERT_EQ(*s3, *s4);
}

TEST(HammingTests, HardSingleDigitTest){
    uint8_t* s1 = new uint8_t[1]{};
    uint8_t* s4 = new uint8_t[1]{};
    s1[0] = 128;
    s4[0] = 128;
    size_t data_size = 8;
    uint8_t* s2 = Encode(s1, 20, data_size);
    uint8_t* s3 = Decode(s2, 20, data_size);
    ASSERT_EQ(*s3, *s4);
}

TEST(HammingTests, MultipleSelfConversionsTest){
    uint8_t* s1 = new uint8_t[1]{};
    uint8_t* s4 = new uint8_t[1]{};
    s1[0] = 128 + 64 + 32;
    s4[0] = 128 + 64 + 32;
    size_t data_size = 8;
    for(int i = 0; i < 1000; ++i) {
         s1 = Encode(s1, 3, data_size);
         s1 = Decode(s1, 3, data_size);
        ASSERT_EQ(*s1, *s4);
    }
}

TEST(HammingTests, SingleDigitCorruptionTest){
    uint8_t* s1 = new uint8_t[1]{};
    uint8_t* s4 = new uint8_t[1]{};
    s1[0] = 128;
    s4[0] = 128;
    size_t data_size = 8;
    uint8_t* s2 = Encode(s1, 3, data_size);
    s2[0] ^= (1 << 5);
    uint8_t* s3 = Decode(s2, 3, data_size);
    ASSERT_EQ(*s3, *s4);
}

TEST(HammingTests, MultipleDigitsTest1){
    int n = 1000;
    uint8_t* s1 = new uint8_t[n]{};
    uint8_t* s4 = new uint8_t[n]{};
    for(int i = 0; i < n; ++i){
        s1[i] = (i * 3) % 255;
        s4[i] = (i * 3) % 255;
    }
    size_t data_size = n * 8;
    uint8_t* s2 = Encode(s1, 3, data_size);
    uint8_t* s3 = Decode(s2, 3, data_size);
    for(int i = 0; i < n; ++i){
        ASSERT_EQ(s3[i], s4[i]);
    }
}

TEST(HammingTests, MultipleDigitsTest2){
    int n = 1000;
    uint8_t* s1 = new uint8_t[n]{};
    uint8_t* s4 = new uint8_t[n]{};
    for(int i = 0; i < n; ++i){
        s1[i] = (i * 3) % 255;
        s4[i] = (i * 3) % 255;
    }
    size_t data_size = n * 8;
    uint8_t* s2 = Encode(s1, 4, data_size);
    uint8_t* s3 = Decode(s2, 4, data_size);
    for(int i = 0; i < n; ++i){
        ASSERT_EQ(s3[i], s4[i]);
    }
}

TEST(HammingTests, MultipleDigitsTest3){
    int n = 1000;
    uint8_t* s1 = new uint8_t[n]{};
    uint8_t* s4 = new uint8_t[n]{};
    for(int i = 0; i < n; ++i){
        s1[i] = (i * 3) % 255;
        s4[i] = (i * 3) % 255;
    }
    size_t data_size = n * 8;
    uint8_t* s2 = Encode(s1, 5, data_size);
    uint8_t* s3 = Decode(s2, 5, data_size);
    for(int i = 0; i < n; ++i){
        ASSERT_EQ(s3[i], s4[i]);
    }
}

TEST(HammingTests, MultipleDigitsCorruptionResolvable1){
    int n = 1000;
    uint8_t* s1 = new uint8_t[n]{};
    uint8_t* s4 = new uint8_t[n]{};
    for(int i = 0; i < n; ++i){
        s1[i] = (i * 3) % 255;
        s4[i] = (i * 3) % 255;
    }
    size_t data_size = n * 8;
    uint8_t* s2 = Encode(s1, 3, data_size);
    for(int i = 0; i < n; ++i){
        s2[i] ^= (1 << i % 8);
    }
    uint8_t* s3 = Decode(s2, 3, data_size);
    for(int i = 0; i < n; ++i){
        ASSERT_EQ(s3[i], s4[i]);
    }
}

TEST(HammingTests, MultipleDigitsCorruptionResolvable2){
    int n = 1000;
    uint8_t* s1 = new uint8_t[n]{};
    uint8_t* s4 = new uint8_t[n]{};
    for(int i = 0; i < n; ++i){
        s1[i] = (i * 3) % 255;
        s4[i] = (i * 3) % 255;
    }
    size_t data_size = n * 8;
    uint8_t* s2 = Encode(s1, 4, data_size);
    for(int i = 0; i < n; i += 3){
        s2[i] ^= (1 << i % 8);
    }
    uint8_t* s3 = Decode(s2, 4, data_size);
    for(int i = 0; i < n; ++i){
        ASSERT_EQ(s3[i], s4[i]);
    }
}

TEST(HammingTests, MultipleDigitsCorruptionUnresolvable){
    uint8_t* s1 = new uint8_t[1]{};
    uint8_t* s4 = new uint8_t[1]{};
    s1[0] = 128;
    s4[0] = 128;
    size_t data_size = 8;
    uint8_t* s2 = Encode(s1, 3, data_size);
    s2[0] ^= (1 << 5);
    s2[0] ^= (1 << 1);
    uint8_t* s3 = Decode(s2, 3, data_size);
    ASSERT_EQ(s3, nullptr);
}