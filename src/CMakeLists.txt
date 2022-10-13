file(GLOB_RECURSE _serial_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/**.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/**.inl
        ${CMAKE_CURRENT_SOURCE_DIR}/**.cpp
        )
add_library(serial STATIC
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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "" FILES ${_serial_sources})
