cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_STANDARD 17)

project(STSL-Lobectomy)

find_package(ITK 6.0.0 REQUIRED)
include(${ITK_USE_FILE})
add_subdirectory(src)

