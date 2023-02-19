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

	"hooks.hpp"
	"hooks.cpp"

	"mc_sdk/structs.hpp"
	"mc_sdk/list_container.hpp"

	"mc_sdk/entt.hpp"
	"mc_sdk/entt.cpp"
	"mc_sdk/string_container.hpp"
	"mc_sdk/string_container.cpp"
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
	mcbre_sdk
	minhook
)
