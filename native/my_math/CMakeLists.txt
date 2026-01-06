add_library(my_math STATIC
        affine.h
        basics.h
        box.h
        color.cpp
        color.h
        frustum.cpp
        frustum.h
        my_math.h
        matrix.cpp
        matrix.h
        quat.h
        sphere.h
        vector.cpp
        vector.h
)

target_include_directories(my_math PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
