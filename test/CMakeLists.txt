Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.6.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

if (NOT MSVC)
    target_compile_options(Catch2WithMain PUBLIC -Wno-unused-parameter)
endif ()

link_libraries(OpenGloves Catch2WithMain)

add_subdirectory(AlphaEncoding)
