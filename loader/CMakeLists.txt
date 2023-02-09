cmake_minimum_required(VERSION 3.18)

project(mcbre_loader)

add_executable(
  ${PROJECT_NAME}
  "main.cpp"
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  CXX_STANDARD 20
  OUTPUT_NAME "load"
  SUFFIX ""
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  mcbre_common_lib
)
