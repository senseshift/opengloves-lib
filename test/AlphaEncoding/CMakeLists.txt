set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
        AlphaEncodingTest
        encode_input.cpp split.cpp
)

set_target_properties(AlphaEncodingTest PROPERTIES UNITY_BUILD OFF)

add_test(AlphaEncoding AlphaEncodingTest)
