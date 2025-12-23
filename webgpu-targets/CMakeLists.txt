set(NVRHI_BUILD_SHARED OFF CACHE BOOL "Build as a shared library" FORCE)

find_package(Threads)
find_package(Dawn REQUIRED)

FetchContent_MakeAvailable(nvrhi)

add_subdirectory(webgpu-util)
add_subdirectory(webgpu-glfw-util)
add_subdirectory(webgpu-nvrhi-util)


add_executable(nvrhi_unit_test main.cpp)
target_link_libraries(
        nvrhi_unit_test
        webgpu_util
        GTest::gtest_main
        dawn::webgpu_dawn
        nvrhi
        nvrhi_webgpu
        glfw
)

gtest_discover_tests(nvrhi_unit_test)

add_executable(glfw_app glfw_app.cpp)

target_link_libraries(
        glfw_app
        my_math
        webgpu_util
        webgpu_glfw_util
        webgpu_nvrhi_util
        dawn::webgpu_dawn
        nvrhi
        nvrhi_webgpu
        glfw
)

add_dependencies(glfw_app shaders)

add_executable(cli_webgpu cli_webgpu.cpp ../cli_app.h)

target_link_libraries(
        cli_webgpu
        dawn::webgpu_dawn
        nvrhi
        nvrhi_webgpu
        Image
        webgpu_util
)