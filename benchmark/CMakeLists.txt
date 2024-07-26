Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

link_libraries(OpenGloves Catch2::Catch2WithMain)

add_executable(
        Benchmark
        bench_alpha_encode.cpp
)

set_target_properties(Benchmark PROPERTIES UNITY_BUILD OFF)
target_compile_features(Benchmark PRIVATE cxx_std_20)
