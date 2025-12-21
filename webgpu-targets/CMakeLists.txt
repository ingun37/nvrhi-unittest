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

add_executable(app app.cpp)

target_link_libraries(
        app
        webgpu_util
        webgpu_glfw_util
        dawn::webgpu_dawn
        nvrhi
        nvrhi_webgpu
        glfw
)


add_executable(cli_webgpu cli_webgpu.cpp ../cli_app.h)

target_link_libraries(
        cli_webgpu
        dawn::webgpu_dawn
        nvrhi
        nvrhi_webgpu
        Image
        webgpu_util
)