add_library(scenario STATIC scenario.h
        scenario.cpp)
target_link_libraries(scenario my_math nvrhi Image)
target_include_directories(scenario PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})