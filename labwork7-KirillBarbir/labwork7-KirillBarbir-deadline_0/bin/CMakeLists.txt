add_executable(${PROJECT_NAME} main.cpp
        ../lib/Backend.cpp
        ../lib/Backend.h
        ../lib/Window.cpp
        ../lib/Window.h)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})