#include "ArgParser.h"

ArgumentParser::ArgParser CreateHammingParser() {
    ArgumentParser::ArgParser parser("HamArc parser");
    parser.AddFlag('c', "create");
    parser.AddFlag('l', "list");
    parser.AddFlag('x', "extract");
    parser.AddFlag('a', "append");
    parser.AddFlag('d', "delete");
    parser.AddFlag('A', "concatenate");
    parser.AddStringArgument("arguments").Positional().MultiValue();
    parser.AddStringArgument('f', "file");
    parser.AddIntArgument('p', "parity").Default(3);//parity
    return parser;
}
