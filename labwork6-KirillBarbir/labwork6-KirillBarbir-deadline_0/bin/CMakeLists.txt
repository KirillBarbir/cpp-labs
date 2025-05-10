add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ArgParser)
target_link_libraries(${PROJECT_NAME} PRIVATE ArgParserConfig)
target_link_libraries(${PROJECT_NAME} PRIVATE FileManager)
target_link_libraries(${PROJECT_NAME} PRIVATE HammingCode)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})