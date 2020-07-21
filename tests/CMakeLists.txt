file(GLOB_RECURSE SERIAL_TEST_HEADER_FILES ".h" "*.hpp")
file(GLOB_RECURSE SERIAL_TEST_SOURCE_FILES ".c" "*.cpp")
set(SERIAL_TEST_SOURCES
		${SERIAL_TEST_HEADER_FILES}
		${SERIAL_TEST_SOURCE_FILES}
		)

add_executable(Serial_Test ${SERIAL_TEST_SOURCES})

target_compile_features(Serial_Test PUBLIC cxx_std_17)
target_include_directories(Serial_Test PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(Serial_Test PUBLIC Serial gtest_main)

set_target_properties(Serial_Test PROPERTIES
		FOLDER "Serial"
		)

add_test(NAME unit
		COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/Serial_Test
		)
add_custom_command(TARGET Serial_Test POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../data ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/data 
		)
