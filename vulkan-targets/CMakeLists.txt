set(NVRHI_BUILD_SHARED ON CACHE BOOL "Build as a shared library" FORCE)


find_package(Vulkan REQUIRED
        MoltenVK
        #        volk
)


FetchContent_MakeAvailable(nvrhi)

add_executable(cli_vulkan cli_vulkan.cpp ../cli_app.h)

target_link_libraries(cli_vulkan
        nvrhi
        #        nvrhi_vk
        Vulkan::Vulkan
        Vulkan::MoltenVK
        #        Vulkan::volk
)
get_filename_component(MOLTENVK_LIB_DIR "${Vulkan_MoltenVK_LIBRARY}" DIRECTORY)
# Set RPATH for build dependencies on macOS
set_target_properties(cli_vulkan PROPERTIES
        INSTALL_RPATH "${MOLTENVK_LIB_DIR};${CMAKE_BINARY_DIR}/_deps/nvrhi-build"
        BUILD_WITH_INSTALL_RPATH TRUE
        MACOSX_RPATH TRUE
)

