file(GLOB_RECURSE SERIAL_HEADER_FILES
		RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
		".h" "*.hpp" "*.inl"
		)
file(GLOB_RECURSE SERIAL_SOURCE_FILES
		RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
		".c" "*.cpp"
		)

add_library(Serial STATIC ${SERIAL_HEADER_FILES} ${SERIAL_SOURCE_FILES})

target_compile_features(Serial PUBLIC cxx_std_17)
target_include_directories(Serial PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

set_target_properties(Serial PROPERTIES
		FOLDER "Serial"
		)
		