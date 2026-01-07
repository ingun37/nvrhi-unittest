add_library(nvrhi-util nvrhi_util.h nvrhi_util.cpp)
target_link_libraries(nvrhi-util nvrhi)
target_include_directories(nvrhi-util PUBLIC .)