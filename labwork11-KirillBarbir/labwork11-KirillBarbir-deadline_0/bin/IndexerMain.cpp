#include "imported_lib/ArgParser.h"
#include "lib/Indexer.h"

int main(int argc, char** argv) {
    ArgumentParser::ArgParser parser("Parser");
    parser.AddStringArgument('i', "i_directory", "A directory to index");
    parser.AddStringArgument('s', "s_directory", "A directory to store index");
    parser.AddStringArgument('n', "name", "A index file name").Default("index");
    parser.Parse(argc, argv);
    IndexClass index(parser.GetStringValue('i'));
    index.WriteData(parser.GetStringValue('n'), parser.GetStringValue('s'));
    return 0;
}