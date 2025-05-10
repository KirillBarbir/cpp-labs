cmake_minimum_required(VERSION 3.28)

project(
        labwork7
        VERSION 1.0
        DESCRIPTION "C++ Year 2024 labwork7"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()


include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 00429da19f55834c912af5581290fc0584f3ca67) # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG 0457de21cffb298c22b629e538036bfeb96130b7)
FetchContent_MakeAvailable(json)

include(FetchContent)

FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG v5.0.0
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()


add_subdirectory(bin)
add_subdirectory(lib)

#include_directories("C:/Program Files/JetBrains/CLion 2023.2.1/PDCurses-3.9/PDCurses-3.9")
#target_link_libraries(${PROJECT_NAME} PUBLIC "C:/Program Files/JetBrains/CLion 2023.2.1/PDCurses-3.9/PDCurses-3.9/libpdcurses.a")

target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component # Not needed for this example.
)
#target_link_libraries(${PROJECT_NAME} PUBLIC ftxui::ftxui)