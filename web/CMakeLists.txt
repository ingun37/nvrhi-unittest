cmake_minimum_required(VERSION 4.1)
project(async_test)

set(CMAKE_CXX_STANDARD 23)
include(FetchContent)


# Configure NVRHI settings before FetchContent (correct approach)
set(NVRHI_WITH_VULKAN OFF CACHE BOOL "Enable Vulkan Backend")
set(NVRHI_WITH_WEBGPU ON CACHE BOOL "Enable WebGPU Backend")
set(NVRHI_WITH_DX11 OFF CACHE BOOL "Disable DX11 Backend" FORCE)
set(NVRHI_WITH_DX12 OFF CACHE BOOL "Disable DX12 Backend" FORCE)
set(NVRHI_WITH_VALIDATION OFF CACHE BOOL "Disable NVRHI Validation Layer" FORCE)
set(NVRHI_BUILD_SHARED OFF CACHE BOOL "Build as a shared library" FORCE)
set(NVRHI_GIT_URL "https://github.com/clo3d/NVRHI.git" CACHE STRING "NVRHI Git repository URL")

# FetchContent declaration without CMAKE_ARGS
FetchContent_Declare(
        nvrhi
        #        GIT_REPOSITORY ${NVRHI_GIT_URL}
        #        GIT_TAG 7b92100074c99cc5e3abbeb3fbdd9f2beabbe778
        SOURCE_DIR /Users/ingun/projects/NVRHI
)
FetchContent_MakeAvailable(nvrhi)

if (NOT EMSCRIPTEN)
    find_package(Threads)
    find_package(Dawn REQUIRED)

    add_executable(async_test main.cpp)
    target_link_libraries(async_test dawn::webgpu_dawn)

else ()
    add_executable(async_test main.cpp)
    target_link_libraries(async_test)
    target_compile_options(async_test PUBLIC --use-port=emdawnwebgpu)
    target_link_options(async_test PUBLIC --use-port=emdawnwebgpu -lembind)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_executable(async_test2 main2.cpp)
    target_link_libraries(async_test2)
    target_compile_options(async_test2 PUBLIC --use-port=emdawnwebgpu)
    target_link_options(async_test2 PUBLIC --use-port=emdawnwebgpu)

    add_executable(async_test3 main3.cpp)
    target_link_libraries(async_test3)
    target_compile_options(async_test3 PUBLIC --use-port=emdawnwebgpu -sASYNCIFY)
    target_link_options(async_test3 PUBLIC --use-port=emdawnwebgpu -sASYNCIFY)

    add_executable(async_test4 main4.cpp)
    target_link_libraries(async_test4)
    target_compile_options(async_test4 PUBLIC --use-port=emdawnwebgpu)
    target_link_options(async_test4 PUBLIC --use-port=emdawnwebgpu)

    add_executable(async_test5 main5.cpp)
    target_link_libraries(async_test5)
    target_compile_options(async_test5 PUBLIC --use-port=emdawnwebgpu -sALLOW_BLOCKING_ON_MAIN_THREAD -pthread -sPTHREAD_POOL_SIZE=2)
    target_link_options(async_test5 PUBLIC --use-port=emdawnwebgpu -sALLOW_BLOCKING_ON_MAIN_THREAD -pthread -sPTHREAD_POOL_SIZE=2)

    add_executable(async_test6 main6.cpp)
    target_link_libraries(async_test6)
    target_compile_options(async_test6 PUBLIC --use-port=emdawnwebgpu -sALLOW_BLOCKING_ON_MAIN_THREAD -pthread -sPTHREAD_POOL_SIZE=2)
    target_link_options(async_test6 PUBLIC --use-port=emdawnwebgpu -sALLOW_BLOCKING_ON_MAIN_THREAD -pthread -sPTHREAD_POOL_SIZE=2)

    add_executable(async_test7 main7.cpp)
    target_link_libraries(async_test7)
    target_compile_options(async_test7 PUBLIC --use-port=emdawnwebgpu -sPROXY_TO_PTHREAD -pthread -sPTHREAD_POOL_SIZE=4)
    target_link_options(async_test7 PUBLIC --use-port=emdawnwebgpu -sPROXY_TO_PTHREAD -pthread -sPTHREAD_POOL_SIZE=4)

    add_executable(main_glfw main_glfw.cpp)
    target_link_libraries(main_glfw
            nvrhi
            nvrhi_webgpu
    )
    target_compile_options(main_glfw PUBLIC --use-port=contrib.glfw3 -lembind)
    target_link_options(main_glfw PUBLIC --use-port=contrib.glfw3 -lembind)

endif ()

