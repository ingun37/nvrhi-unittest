include(FetchContent)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 8d1fd52e5ab5590e2c81768ace50c72bae28f2ed #refs/tags/1.0.3
)

FetchContent_MakeAvailable(glm)

add_library(scenario STATIC
        include/scenario/scenario.h
        include/scenario/Step.h
        include/scenario/Context.h
        include/scenario/ChooseScenario.h
        ChooseScenario.cpp
        backend.h
        Map3DStagingMipMap.cpp
        Map3DStagingMipMap.h
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
        ConstantBlend.cpp
        ConstantBlend.h
        DepthBias.cpp
        DepthBias.h
        DepthBiasSlopeScale.cpp
        DepthBiasSlopeScale.h
        DynamicRender.cpp
        DynamicRender.h
)
target_link_libraries(scenario my_math nvrhi Image my_io nvrhi-util glm::glm)
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
