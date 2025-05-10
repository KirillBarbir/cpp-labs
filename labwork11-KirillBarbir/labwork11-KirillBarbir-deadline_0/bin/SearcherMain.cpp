#include "imported_lib/ArgParser.h"
#include "lib/Searcher.h"

int main(int argc, char** argv) {
    ArgumentParser::ArgParser parser("Parser");
    parser.AddStringArgument('i', "index", "A path to file, produced by indexer");
    parser.AddIntArgument('k', "K-top", "Amount of most relevant files to find").Default(100);
    parser.AddFlag("occurrences", "Need to get occurrences?").Default(false);
    parser.Parse(argc, argv);
    IndexClass index;
    index.ReadData(parser.GetStringValue('i'));
    while (1) {
        std::string query;
        std::getline(std::cin, query);
        if (query == " ") {
            continue;
        }
        QueryTree searcher;
        searcher.ParseQueryCountAndOutput(index,
                                          query,
                                          parser.GetIntValue('k'),
                                          parser.GetFlag("occurrences"));
    }
}
