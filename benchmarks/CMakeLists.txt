include(FetchContent)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
FetchContent_Declare(
  googlebenchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.6.1.tar.gz
)

FetchContent_MakeAvailable(googlebenchmark)

add_executable(
    benchmarks
    main.cxx
)

target_link_libraries(benchmarks PRIVATE fsmpp2 benchmark::benchmark benchmark::benchmark_main)