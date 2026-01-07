add_library(scenario STATIC
        include/scenario/scenario.h
        scenario.cpp
        App.h
        Copy2DTexture.h
        Copy2DTexture.cpp
        include/scenario/Context.h
        Copy3D.cpp
        Copy3D.h
        Map2DStaging.cpp
        Map2DStaging.h
        Map3DStaging.cpp
        Map3DStaging.h
        Shader.cpp
        Shader.h)
target_link_libraries(scenario my_math nvrhi Image my_io nvrhi-util)
target_include_directories(scenario PUBLIC include PRIVATE .)