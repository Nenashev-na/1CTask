cmake_minimum_required(VERSION 3.17)
project(1CTask)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
set(SOURCE_FILES main.cpp)
add_executable(1CTask ${SOURCE_FILES})
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(1CTask ${OpenCV_LIBS})