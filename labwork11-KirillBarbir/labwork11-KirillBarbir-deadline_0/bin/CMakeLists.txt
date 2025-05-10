add_executable(Index IndexerMain.cpp
        ../imported_lib/ArgParser.cpp)
add_executable(Search SearcherMain.cpp
        ../imported_lib/ArgParser.cpp)

target_link_libraries(Index PRIVATE Indexer)
target_link_libraries(Search PRIVATE Searcher)
target_link_libraries(Search PRIVATE Indexer)

target_include_directories(Index PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(Search PUBLIC ${PROJECT_SOURCE_DIR})