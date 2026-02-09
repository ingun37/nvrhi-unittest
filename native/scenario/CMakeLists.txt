add_library(scenario STATIC
        include/scenario/scenario.h
        include/scenario/App.h
        include/scenario/Context.h
        backend.h
        Map3DStagingMipMap.cpp
        Map3DStagingMipMap.h
        ChooseApp.cpp
        ChooseApp.h
        scenario.cpp
        MapStagingAsync.cpp
        MapStagingAsync.h
        RenderPassColorClearDraw.cpp
        RenderPassColorClearDraw.h
        RenderPassDepthOnly.cpp
        RenderPassDepthOnly.h
        ConstantBuffer.cpp
        ConstantBuffer.h
        TextureOver16.cpp
        TextureOver16.h
)
target_link_libraries(scenario my_math nvrhi Image my_io nvrhi-util)
target_include_directories(scenario PUBLIC include PRIVATE .)
add_dependencies(scenario shaders)

set(SHADERS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders/generated")
set(SHADERS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders/generated" PARENT_SCOPE)
add_compile_definitions(SCENARIO_SHADERS_OUTPUT_DIR="${SHADERS_OUTPUT_DIR}")
if (NVRHI_WITH_VULKAN)
    add_compile_definitions(SCENARIO_VULKAN=true)
elseif (NVRHI_WITH_WEBGPU)
    add_compile_definitions(SCENARIO_WGPU=true)
endif ()