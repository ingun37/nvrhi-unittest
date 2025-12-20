cmake_minimum_required(VERSION 4.1)
project(nvrhi_unit_test)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
find_package(Threads)

find_package(Dawn REQUIRED)
find_package(glfw3 REQUIRED)

# Configure NVRHI settings before FetchContent (correct approach)
set(NVRHI_WITH_VULKAN OFF CACHE BOOL "Enable Vulkan Backend" FORCE)
set(NVRHI_WITH_WEBGPU ON CACHE BOOL "Enable WebGPU Backend" FORCE)
set(NVRHI_WITH_DX11 OFF CACHE BOOL "Disable DX11 Backend" FORCE)
set(NVRHI_WITH_DX12 OFF CACHE BOOL "Disable DX12 Backend" FORCE)
set(NVRHI_BUILD_SHARED OFF CACHE BOOL "Build as a shared library" FORCE)
set(NVRHI_WITH_VALIDATION OFF CACHE BOOL "Disable NVRHI Validation Layer" FORCE)

set(NVRHI_GIT_URL "https://github.com/clo3d/NVRHI.git" CACHE STRING "NVRHI Git repository URL")

# FetchContent declaration without CMAKE_ARGS
FetchContent_Declare(
        nvrhi
        #        GIT_REPOSITORY ${NVRHI_GIT_URL}
        #        GIT_TAG 7b92100074c99cc5e3abbeb3fbdd9f2beabbe778
        SOURCE_DIR /Users/ingun/projects/NVRHI
)
FetchContent_MakeAvailable(nvrhi)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(nvrhi_unit_test main.cpp
        util.cpp
        util.h
        metal.mm
        metal.h
        stb_image.h
        helper.cpp
        helper.h
)

target_link_libraries(
        nvrhi_unit_test
        GTest::gtest_main
        dawn::webgpu_dawn
        nvrhi
        nvrhi_webgpu
        glfw
        "-framework Metal"
        "-framework QuartzCore"
)

include(GoogleTest)

gtest_discover_tests(nvrhi_unit_test)