add_library(scenario STATIC
        include/scenario/scenario.h
        scenario.cpp
        App.h
        Triangle.h
        Copy2DTexture.h
        Copy2DTexture.cpp
        Triangle.cpp
        include/scenario/Context.h)
target_link_libraries(scenario my_math nvrhi Image)
target_include_directories(scenario PUBLIC include PRIVATE .)