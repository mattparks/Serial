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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)

if(SERIAL_BUILD_EXAMPLES)
    add_executable(serial_example
            ${CMAKE_CURRENT_SOURCE_DIR}/example/Main.cpp
            )
    target_include_directories(serial_example PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/example>)
    target_link_libraries(serial_example PUBLIC serial)
endif()

if(SERIAL_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG v1.7.0
            )

    FetchContent_GetProperties(benchmark)

    if(NOT benchmark_POPULATED)
        FetchContent_Populate(benchmark)
        #set(gtest_force_shared_crt ON CACHE BOOL "Use shared (DLL) run-time lib even when Google Test is built as static lib.")
        set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Enable testing of the benchmark library.")
        add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR})
    endif()

    file(GLOB_RECURSE _serial_test_sources
            ${CMAKE_CURRENT_SOURCE_DIR}/test/**.h
            ${CMAKE_CURRENT_SOURCE_DIR}/test/**.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/**.c
            ${CMAKE_CURRENT_SOURCE_DIR}/test/**.cpp
            )
    set(SERIAL_TEST_SOURCES
            ${_serial_test_sources}
            )
    add_executable(serial_test ${SERIAL_TEST_SOURCES})
    target_include_directories(serial_test PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test>)
    target_link_libraries(serial_test PUBLIC serial benchmark::benchmark)

    # Allows automation of "BUILD_TESTING"
    #include(CTest)
    #enable_testing()
    #add_test(NAME unit
    #        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/serial_test
    #        )
    add_custom_command(TARGET serial_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/data 
            )    
endif()
