add_library(scenario STATIC
        include/scenario/scenario.h
        include/scenario/App.h
        include/scenario/Context.h
        Map3DStagingMipMap.cpp
        Map3DStagingMipMap.h
        ChooseApp.cpp
        ChooseApp.h
        ReadRect.cpp
        ReadRect.h
        ReadBox.cpp
        ReadBox.h
        ReadExtent3D.cpp
        ReadExtent3D.h
        ReadOrigin3D.cpp
        ReadOrigin3D.h
        Termination.cpp
        Termination.h
        scenario.cpp
        MapStagingAsync.cpp
        MapStagingAsync.h)
target_link_libraries(scenario my_math nvrhi Image my_io nvrhi-util)
target_include_directories(scenario PUBLIC include PRIVATE .)