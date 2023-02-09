cmake_minimum_required(VERSION 3.18)

if (NOT TARGET mcbre_common_lib)
	message(FATAL_ERROR "Missing common library target.")
endif()

project(
	mcbre_client
)

add_library(
  ${PROJECT_NAME}
	SHARED
	"main.cpp"
)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
	CXX_STANDARD 20	
	OUTPUT_NAME "client"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/"
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	mcbre_common_lib
	minhook
)
