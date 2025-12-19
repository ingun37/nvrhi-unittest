cmake_minimum_required(VERSION 4.1)
project(nvrhi_unit_test)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(nvrhi_unit_test main.cpp)

target_link_libraries(
        nvrhi_unit_test
        GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(nvrhi_unit_test)