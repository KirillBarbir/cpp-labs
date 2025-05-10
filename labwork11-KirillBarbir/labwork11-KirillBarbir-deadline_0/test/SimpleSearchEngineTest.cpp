#include <gtest/gtest.h>
#include <strstream>
#include "lib/Searcher.h"

const std::string TEST_DATA_directory = "C:\\gitprojects\\labwork11-KirillBarbir\\test\\TEST_DATA";

TEST(SimpleSearchEngineTests, directory1_3_simple_test1) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks\\1-3");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "getopt", 1, false, out);
    ASSERT_EQ(out.str(),
              "Most relevant documents:\n1) " + TEST_DATA_directory +
                  "\\labworks\\1-3\\labwork1-KirillBarbir\\main.cpp\n\n");
}

TEST(SimpleSearchEngineTests, directory1_3_simple_test2) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks\\1-3");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "int2023", 1, false, out);
    ASSERT_EQ(out.str(),
              "Most relevant documents:\n1) " + TEST_DATA_directory +
                  "\\labworks\\1-3\\labwork2-KirillBarbir\\lib\\number.h\n\n");
}

TEST(SimpleSearchEngineTests, directory1_3_simple_test3) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks\\1-3");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "BMP AND bitmap", 1, false, out);
    ASSERT_EQ(out.str(),
              "Most relevant documents:\n1) " + TEST_DATA_directory +
                  "\\labworks\\1-3\\labwork3-KirillBarbir\\lib\\BMPOutput.cpp\n\n");
}

TEST(SimpleSearchEngineTests, directory4_6_simple_test1) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks\\4-6");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "size", 1, false, out);
    ASSERT_EQ(out.str(),
              "Most relevant documents:\n1) " + TEST_DATA_directory +
                  "\\labworks\\4-6\\labwork6-KirillBarbir\\bin\\main.cpp\n\n");
}

TEST(SimpleSearchEngineTests, directory7_10_simple_test1) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks\\7-10");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "template OR bitmap", 1, false, out);
    ASSERT_EQ(out.str(),
              "Most relevant documents:\n1) " + TEST_DATA_directory +
                  "\\labworks\\7-10\\labwork9-KirillBarbir\\lib\\Adapter.h\n\n");
}

TEST(SimpleSearchEngineTests, directory_labwork_simple_test1) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "BMP AND bitmap", 1, false, out);
    ASSERT_EQ(out.str(),
              "Most relevant documents:\n1) " + TEST_DATA_directory +
                  "\\labworks\\1-3\\labwork3-KirillBarbir\\lib\\BMPOutput.cpp\n\n");
}

TEST(SimpleSearchEngineTests, directory_labwork_simple_test2) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "parser", 1, false, out);
    ASSERT_EQ(out.str(),
              "Most relevant documents:\n1) " + TEST_DATA_directory +
                  "\\labworks\\4-6\\labwork4-KirillBarbir\\tests\\argparser_test.cpp\n\n");
}

TEST(SimpleSearchEngineTests, directory_labwork_simple_test3) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "template", 1, false, out);
    ASSERT_EQ(out.str(),
              "Most relevant documents:\n1) " + TEST_DATA_directory +
                  "\\labworks\\7-10\\labwork9-KirillBarbir\\lib\\Adapter.h\n\n");
}

TEST(SimpleSearchEngineTests, directory_labwork_hard_test1) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "((((template AND using) OR (bitmap AND BMP))))", 2, false, out);
    ASSERT_EQ(out.str(),
              "Most relevant documents:\n1) " + TEST_DATA_directory +
                  "\\labworks\\1-3\\labwork3-KirillBarbir\\lib\\BMPOutput.cpp\n2) "
                  + TEST_DATA_directory + "\\labworks\\7-10\\labwork9-KirillBarbir\\lib\\Adapter.h\n\n");
}

TEST(SimpleSearchEngineTests, directory_labwork_hard_test2) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "((((template OR using) AND (adapter OR bst))))", 10, false, out);
    ASSERT_EQ(out.str(),
              "Most relevant documents:\n1) " + TEST_DATA_directory +
                  "\\labworks\\7-10\\labwork8-KirillBarbir\\lib\\BST.h\n\n");
}

TEST(SimpleSearchEngineTests, output_lines) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "yellow", 1, true, out);
    ASSERT_EQ(out.str(),
              "Most relevant documents:\n1) " + TEST_DATA_directory +
                  "\\labworks\\1-3\\labwork3-KirillBarbir\\lib\\BMPOutput.cpp\n"
                  "Occurrences:\n"
                  "yellow (1 in total):\n"
                  "1 in line 16\n\n");
}

TEST(SimpleSearchEngineTests, special_cases1) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "yellow red purple", 1, true, out);
    ASSERT_EQ(out.str(), "");
}

TEST(SimpleSearchEngineTests, special_cases2) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "yellow OR OR OR ", 1, true, out);
    ASSERT_EQ(out.str(), "");
}

TEST(SimpleSearchEngineTests, special_cases3) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "CantFindThis", 1, true, out);
    ASSERT_EQ(out.str(), "No documents found! :(\n\n");
}

TEST(SimpleSearchEngineTests, special_cases4) {
    IndexClass indexw(TEST_DATA_directory + "\\labworks");
    indexw.WriteData("index", TEST_DATA_directory);
    IndexClass indexr;
    indexr.ReadData(TEST_DATA_directory + "\\index");

    std::stringstream out;
    QueryTree searcher;
    searcher.ParseQueryCountAndOutput(indexr, "Bitmap AND template", 1, true, out);
    ASSERT_EQ(out.str(), "No documents found! :(\n\n");
}