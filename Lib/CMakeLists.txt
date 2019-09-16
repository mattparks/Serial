file(GLOB_RECURSE SERIAL_HEADER_FILES ".h" "*.hpp")
file(GLOB_RECURSE SERIAL_SOURCE_FILES ".c" "*.cpp")
set(SERIAL_SOURCES
		${SERIAL_HEADER_FILES}
		${SERIAL_SOURCE_FILES}
		)

add_library(Serial STATIC ${SERIAL_SOURCES})

target_compile_features(Serial PUBLIC cxx_std_17)
target_include_directories(Serial PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

set_target_properties(Serial PROPERTIES
		FOLDER "Serial"
		)
		