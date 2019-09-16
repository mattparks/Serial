file(GLOB_RECURSE SERIAL_EXAMPLE_HEADER_FILES ".h" "*.hpp")
file(GLOB_RECURSE SERIAL_EXAMPLE_SOURCE_FILES ".c" "*.cpp")
set(SERIAL_EXAMPLE_SOURCES
		${SERIAL_EXAMPLE_HEADER_FILES}
		${SERIAL_EXAMPLE_SOURCE_FILES}
		)

add_executable(Serial_Example ${SERIAL_EXAMPLE_SOURCES})

target_compile_features(Serial_Example PUBLIC cxx_std_17)
target_include_directories(Serial_Example PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(Serial_Example PUBLIC Serial)

set_target_properties(Serial_Example PROPERTIES
		FOLDER "Serial"
		)
		