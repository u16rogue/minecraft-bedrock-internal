cmake_minimum_required(VERSION 3.18)

project(
	mcbre_common_lib
)

add_library(
	${PROJECT_NAME}
	STATIC
	"include/common/logging.hpp"
	"src/logging.cpp"

	"include/common/metapp.hpp"
	"include/common/pattern_scanner.hpp"

	"include/common/fnvhash.hpp"
	"src/fnvhash.cpp"
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	"include/"
)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
	CXX_STANDARD 20
)
