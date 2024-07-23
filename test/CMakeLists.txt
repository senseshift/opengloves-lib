set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(catch)

link_libraries(OpenGloves catch)

add_subdirectory(AlphaEncoding)