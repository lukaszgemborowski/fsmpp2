project(fsmpp2)
cmake_minimum_required(VERSION 3.16)

option(FSMPP2_USE_CPP20 "Use C++20 features (requires C++20 capable compiler)" OFF)
option(FSMPP2_TEST_ASAN "Enable ASan in tests" OFF)
option(FSMPP2_TEST_UBSAN "Enable UBSan in tests" OFF)

set(FSMPP2_VERSION_MAJOR 0)
set(FSMPP2_VERSION_MINOR 3)
set(FSMPP2_VERSION_PATCH 0)

configure_file(
    cmake/config.hpp.in
    include/fsmpp2/config.hpp
)

add_library(fsmpp2 INTERFACE)
target_include_directories(
    fsmpp2 INTERFACE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include # configure_file will place config.hpp in BINARY_DIR
)

# enable either C++17 or C++20 compiler options
if (FSMPP2_USE_CPP20)
    target_compile_features(fsmpp2 INTERFACE cxx_std_20)
else()
    target_compile_features(fsmpp2 INTERFACE cxx_std_17)
endif()

# install all headers
install(
    DIRECTORY include/fsmpp2
    DESTINATION include
)

# explicitly install aut-generated config.hpp which is out-of-tree
install(
    FILES ${CMAKE_BINARY_DIR}/include/fsmpp2/config.hpp
    DESTINATION include/fsmpp2/
)

add_subdirectory(tests)
add_subdirectory(examples)
add_subdirectory(benchmarks)