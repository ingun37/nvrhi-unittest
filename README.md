Follow [Dawn CMake Quickstart](https://dawn.googlesource.com/dawn/+/refs/heads/main/docs/quickstart-cmake.md)

```sh
cd dawn
cmake -S . -B out/Release -DDAWN_FETCH_DEPENDENCIES=ON -DDAWN_ENABLE_INSTALL=ON -DCMAKE_BUILD_TYPE=Release
cmake --build out/Release
cmake --install out/Release --prefix install/Release
```

Set Dawn_DIR to the `{project root}/dawn/install/Release/lib/cmake/Dawn` 