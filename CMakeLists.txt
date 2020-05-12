cmake_minimum_required(VERSION 2.8)
project(hcsstat)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${ROOT}/bin)
set(CMAKE_CXX_STANDARD 11)

include_directories(
    ${ROOT}/include
)

file(GLOB_RECURSE CppFiles ${ROOT}/src/*.cpp)
file(GLOB_RECURSE HppFiles ${ROOT}/include/*.hpp)

add_executable(hcsstat ${CppFiles} ${HppFiles})
