cmake_minimum_required(VERSION 3.6)
project(VideoCompare CXX)
set(CMAKE_CONFIGURATION_TYPES
    Release RelWithDebInfo
    CACHE STRING INTERNAL FORCE)
find_package(OpenCV REQUIRED)
set(SRC "main.cpp" "icon.rc")
add_executable(VideoCompare ${SRC})
target_link_libraries(VideoCompare PRIVATE ${OpenCV_LIBS})
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Zi /Od")