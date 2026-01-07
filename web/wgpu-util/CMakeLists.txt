add_library(wgpu-util STATIC
        wgpu-util.cpp include/wgpu-util.h)
target_link_libraries(wgpu-util nvrhi nvrhi_webgpu my_math)
target_include_directories(wgpu-util PUBLIC include PRIVATE .)