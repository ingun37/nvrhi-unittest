add_library(webgpu_glfw_util STATIC webgpu-glfw-util.cpp webgpu-glfw-util.h metal.mm)
target_include_directories(webgpu_glfw_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(webgpu_glfw_util
        glfw
        dawn::webgpu_dawn
        "-framework Metal"
        "-framework QuartzCore"
)