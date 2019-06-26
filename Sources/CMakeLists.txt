file(GLOB_RECURSE SERIAL_HEADER_FILES "${CMAKE_SOURCE_DIR}/Sources/*.hpp")
file(GLOB_RECURSE SERIAL_SOURCE_FILES "${CMAKE_SOURCE_DIR}/Sources/*.cpp")
set(SERIAL_SOURCES
		${SERIAL_HEADER_FILES}
		${SERIAL_SOURCE_FILES}
		)

add_executable(Serial ${SERIAL_SOURCES})

target_compile_features(Serial PUBLIC cxx_std_17)
target_include_directories(Serial PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

set_target_properties(Serial PROPERTIES
		FOLDER "Serial"
		)
