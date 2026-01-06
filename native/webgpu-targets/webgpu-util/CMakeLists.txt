add_library(webgpu_util STATIC webgpu-util.cpp webgpu-util.h)
target_include_directories(webgpu_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(webgpu_util
        dawn::webgpu_dawn
)