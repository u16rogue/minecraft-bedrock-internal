cmake_minimum_required(VERSION 3.18)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1) # for clangd

# Project internal name
add_compile_definitions(MCBRE_NAME="kamabato")
add_compile_definitions(MCBRE_PROJ_DIR_NAME="minecraft-bedrock-internal")

# Logging toggle
option(MCBRE_LOGGING "Enable console logging" ON)
if (MCBRE_LOGGING)
  add_compile_definitions(MCBRE_LOGGING=1)
else()
  add_compile_definitions(MCBRE_LOGGING=0)
endif()

# Head Projects
project(
  mcbre
)

# Dependencies

# Projects
add_subdirectory("common/")
add_subdirectory("loader/")
add_subdirectory("client/")
