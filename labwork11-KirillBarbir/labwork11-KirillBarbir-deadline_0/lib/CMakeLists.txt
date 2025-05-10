add_library(Indexer Indexer.h
        Indexer.cpp)
add_library(Searcher Searcher.h
        Searcher.cpp)

set_target_properties(Indexer PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Searcher PROPERTIES LINKER_LANGUAGE CXX)