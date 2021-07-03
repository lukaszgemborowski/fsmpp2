project(fsmpp2)
cmake_minimum_required(VERSION 3.16)

add_library(fsmpp2 INTERFACE)
target_include_directories(fsmpp2 INTERFACE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(fsmpp2 INTERFACE cxx_std_20)

add_subdirectory(tests)