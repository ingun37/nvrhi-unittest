add_library(Image STATIC Image.cpp Image.h stb_image.h)
target_include_directories(Image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})