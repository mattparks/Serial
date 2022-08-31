# 3.11.0 required for cxx_std_17 and FetchContent
cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)
project(serial VERSION 0.1.0 LANGUAGES C CXX)

option(BUILD_SHARED_LIBS "Build Shared Libraries" ON)
option(SERIAL_BUILD_EXAMPLES "Build the Serial example" ON)
option(SERIAL_BUILD_TESTS "Build test applications" ON)

# Sets the install directories defined by GNU
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# Add property to allow making project folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# Position-independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to ON
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
# Under some compilers CMAKE_DEBUG_POSTFIX is set to "d", removed to clean dll names
set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "Set Debug library postfix")
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "Set Release library postfix")
set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "Set RelWithDebInfo library postfix")
# Removes any dll prefix name on windows, unix will keep a prefix set as "lib"
if(WIN32)
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

file(GLOB_RECURSE _serial_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/**.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/**.inl
        )
file(GLOB_RECURSE _serial_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp
        )
add_library(serial STATIC
        ${_serial_headers}
        ${_serial_sources}
        )
target_compile_features(serial PUBLIC cxx_std_17)
target_compile_definitions(serial PUBLIC
        $<$<PLATFORM_ID:Windows>:UNICODE _CRT_SECURE_NO_WARNINGS _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING>
        $<$<CXX_COMPILER_ID:GNU>:__USE_MINGW_ANSI_STDIO=0>
        )
target_compile_options(serial PUBLIC
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wno-unused-function>
        )
target_include_directories(serial PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "" FILES ${_serial_headers})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "" FILES ${_serial_sources})

if(SERIAL_BUILD_EXAMPLES)
    add_executable(serial_example
            ${CMAKE_CURRENT_SOURCE_DIR}/examples/Main.cpp
            )
    target_include_directories(serial_example PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/examples>)
    target_link_libraries(serial_example PUBLIC serial)
endif()

if(SERIAL_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.8.1
            )

    FetchContent_GetProperties(googletest)

    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        set(gtest_force_shared_crt ON CACHE BOOL "Use shared (DLL) run-time lib even when Google Test is built as static lib.")
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
    endif()

    file(GLOB_RECURSE _serial_test_headers
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/**.h
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/**.hpp
            )
    file(GLOB_RECURSE _serial_test_sources
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/**.c
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/**.cpp
            )
    set(SERIAL_TEST_SOURCES
            ${_serial_test_headers}
            ${_serial_test_sources}
            )
    add_executable(serial_test ${SERIAL_TEST_SOURCES})
    target_compile_features(serial_test PUBLIC cxx_std_17)
    target_include_directories(serial_test PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests>)
    target_link_libraries(serial_test PUBLIC serial gtest_main)

    # Allows automation of "BUILD_TESTING"
    include(CTest)
    enable_testing()
    add_test(NAME unit
            COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/serial_test
            )
    add_custom_command(TARGET serial_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/data 
            )    
endif()
