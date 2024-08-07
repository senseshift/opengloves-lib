cmake_minimum_required(VERSION 3.12)

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
endif ()

project(
        "OpenGloves"
        VERSION 0.1.0
        LANGUAGES CXX
)

include(GNUInstallDirs)

add_library(OpenGloves INTERFACE)
add_library(OpenGloves::OpenGloves ALIAS OpenGloves)

target_include_directories(
        OpenGloves INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(OpenGloves INTERFACE cxx_std_17)
if (MSVC)
    # target_compile_options(OpenGloves INTERFACE /W4 /WX)
else ()
    target_compile_options(OpenGloves INTERFACE -Wall -Wextra -Werror)
endif ()

# locations are provided by GNUInstallDirs
install(
        TARGETS
        OpenGloves
        EXPORT
        OpenGlovesTargets
        ARCHIVE DESTINATION
        ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION
        ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION
        ${CMAKE_INSTALL_BINDIR}
)

include(CMakePackageConfigHelpers)

if (${CMAKE_VERSION} VERSION_GREATER "3.14.0")
    set(ARCH_INDEPENDENT "ARCH_INDEPENDENT")
endif ()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif ()

# Testing
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
    add_subdirectory(benchmark)
endif ()

install(
        TARGETS OpenGloves
        EXPORT OpenGloves_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
