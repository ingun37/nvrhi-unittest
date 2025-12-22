Follow [Dawn CMake Quickstart](https://dawn.googlesource.com/dawn/+/refs/heads/main/docs/quickstart-cmake.md)

```sh
cd dawn
cmake -S . -B out/Release -DDAWN_FETCH_DEPENDENCIES=ON -DDAWN_ENABLE_INSTALL=ON -DCMAKE_BUILD_TYPE=Release
cmake --build out/Release
cmake --install out/Release --prefix install/Release
```

Set Dawn_DIR to the `{project root}/dawn/install/Release/lib/cmake/Dawn`

## Enable Metal debugger for MoltenVK backend build

Comment out

```c++
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
```

this part in the `vulkan-backend.h` from NVRHI repo

In Xcode,

1. Go to *Edit Scheme -> Run -> Options*
2. Set *GPU Frame Capture* to `Metal`
3. (Maybe unnecessary) Go to *Build Setting* and set *Metal Capture Enabled* to `YES`