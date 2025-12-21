add_library(webgpu_nvrhi_util STATIC webgpu-nvrhi-util.cpp webgpu-nvrhi-util.h)
target_include_directories(webgpu_nvrhi_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(webgpu_nvrhi_util
        glfw
        dawn::webgpu_dawn
        nvrhi
        nvrhi_webgpu
)