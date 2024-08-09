add_executable(
        AlphaEncodingTest
        encode_input.cpp
        split.cpp
        decode_output.cpp
        encode_output.cpp
)

set_target_properties(AlphaEncodingTest PROPERTIES UNITY_BUILD OFF)

target_compile_features(AlphaEncodingTest PRIVATE cxx_std_20)

add_test(AlphaEncoding AlphaEncodingTest)

include(../../cmake/CheckCoverage.cmake)
target_check_coverage(AlphaEncodingTest)