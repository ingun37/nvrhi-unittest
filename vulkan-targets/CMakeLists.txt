find_package(Vulkan REQUIRED MoltenVK)

FetchContent_MakeAvailable(nvrhi)

add_executable(cli_vulkan cli_vulkan.cpp ../cli_app.h)

target_link_libraries(cli_vulkan
        nvrhi
        Vulkan::Vulkan
)