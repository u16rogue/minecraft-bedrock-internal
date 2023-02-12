cmake_minimum_required(VERSION 3.18)

project(mcbre_sdk)

add_library(
  ${PROJECT_NAME}
  STATIC
  "include/sdk/vec.hpp"
  "src/vec.cpp"
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  CXX_STANDARD 20
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  "include/"
)
