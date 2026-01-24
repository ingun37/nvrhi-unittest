find_package(Vulkan COMPONENTS
        MoltenVK
)


FetchContent_MakeAvailable(nvrhi)

add_executable(cli_vulkan cli_vulkan.cpp)
target_link_libraries(cli_vulkan

        scenario
        nvrhi
        Vulkan::Vulkan
        Vulkan::MoltenVK
        Image
)

#target_link_options(cli_vulkan
#        PRIVATE
#        "-undefined" "dynamic_lookup"
#)
get_filename_component(MOLTENVK_LIB_DIR "${Vulkan_MoltenVK_LIBRARY}" DIRECTORY)
# Set RPATH for build dependencies on macOS
set_target_properties(cli_vulkan PROPERTIES
        INSTALL_RPATH "${MOLTENVK_LIB_DIR};${CMAKE_BINARY_DIR}/_deps/nvrhi-build"
        BUILD_WITH_INSTALL_RPATH TRUE
        MACOSX_RPATH TRUE
)

