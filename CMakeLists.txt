cmake_minimum_required(VERSION 3.13)
project(burst)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

find_package(OpenCL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/lib)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(lib)
add_subdirectory(src)

target_compile_definitions(burst PRIVATE CL_HPP_TARGET_OPENCL_VERSION=300)
target_link_options(burst PRIVATE -lglfw -lGL -lOpenCL)