cmake_minimum_required(VERSION 3.18)

project(mcbre_sdk)

add_library(
  ${PROJECT_NAME}
  STATIC
  "include/sdk/sdk_interface.hpp"
  "include/sdk/plugin_interface.hpp"
  "include/sdk/module_interface.hpp"
  "include/sdk/client_interface.hpp"
  "include/sdk/helper.hpp"
  "src/dummy.cpp"
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  CXX_STANDARD 20
  LINKER_LANGUAGE CXX
)

target_include_directories(
  ${PROJECT_NAME}
  INTERFACE
  "include/"
)
