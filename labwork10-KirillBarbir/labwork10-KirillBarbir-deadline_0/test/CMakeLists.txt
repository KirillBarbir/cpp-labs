include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        TTaskSheduler_test
        TTaskSheduler_test.cpp
)

target_link_libraries(
        TTaskSheduler_test
        TTaskSheduler
        GTest::gtest_main
)

target_include_directories(TTaskSheduler_test PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(TTaskSheduler_test)