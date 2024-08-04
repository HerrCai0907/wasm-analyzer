cmake_minimum_required(VERSION 3.20)
project(wasm-analyzer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
add_subdirectory(third_party)
