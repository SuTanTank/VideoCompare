cmake_minimum_required(VERSION 3.6)
project(VideoCompare CXX)
set(CMAKE_CONFIGURATION_TYPES
    Release RelWithDebInfo
    CACHE STRING INTERNAL FORCE)
set(CMAKE_CXX_STANDARD 14)
list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})
find_package(OpenCV REQUIRED CONFIG)
set(SRC "main.cpp" "icon.rc")
add_executable(VideoCompare MACOSX_BUNDLE ${SRC})
target_link_libraries(VideoCompare PRIVATE ${OpenCV_LIBS})
set_target_properties( VideoCompare
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Zi /Od")