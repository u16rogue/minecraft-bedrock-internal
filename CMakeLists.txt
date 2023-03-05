cmake_minimum_required(VERSION 3.18)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1) # for clangd

# Project internal name
add_compile_definitions(MCBRE_NAME="kamabato")
add_compile_definitions(MCBRE_PROJ_DIR="${CMAKE_SOURCE_DIR}")

message("-- mcbre > Source directory @ ${CMAKE_SOURCE_DIR}")

# Logging toggle
option(MCBRE_LOGGING "Enable console logging" ON)
if (MCBRE_LOGGING)
  add_compile_definitions(MCBRE_LOGGING=1)
  message("-- mcbre > Logging is enabled")
else()
  add_compile_definitions(MCBRE_LOGGING=0)
  message("-- mcbre > Logging is disabled")
endif()


set(MCBRE_DIRECTX_PATH "" CACHE PATH "Path to the installed DirectX SDK")
set(MCBRE_WDK_PATH "C:/Program Files (x86)/Windows Kits" CACHE PATH "Path to the install Windows SDK")
set(MCBRE_WDK_PLATFORM "10" CACHE PATH "Platform version of the WDK to use")
set(MCBRE_WDK_VERSION "10.0.22621.0" CACHE PATH "Version of Windows SDK to use")
option(MCBRE_DONT_CARE_WIN_DEPS "Last ditch effort when these dependencies are actually resolved by cmake itself so you dont have to waste your time setting up cmake variables." OFF)

if (NOT MCBRE_DONT_CARE_WIN_DEPS)
  # --- Direct X
  if (MCBRE_DIRECTX_PATH STREQUAL "")
    if (NOT DEFINED ENV{DXSDK_DIR})
      message(FATAL_ERROR "-- mcbre > DXSDK_DIR env was not found. Please manually set MCBRE_DIRECTX_PATH in your CMakeCache.txt or through -DMCBRE_DIRECTX_PATH=path")
    endif()
    set(MCBRE_DIRECTX_PATH "$ENV{DXSDK_DIR}")
  endif()

  if (NOT EXISTS "${MCBRE_DIRECTX_PATH}/Include/D3D11.h")
    message(FATAL_ERROR "-- mcbre > DirectX SDK path was not found. (${MCBRE_DIRECTX_PATH})")
  endif()

  message("-- mcbre > DirectX SDK @ ${MCBRE_DIRECTX_PATH}")
  # ---  Windows SDK
  if (NOT EXISTS "${MCBRE_WDK_PATH}/${MCBRE_WDK_PLATFORM}/Include/${MCBRE_WDK_VERSION}/um/Windows.h")
    message(FATAL_ERROR "-- mcbre > Failed to find Windows.h from the MCBRE_WDK_* parameters. Please properly setup the MCBRE_WDK_* variables.")
  endif()

  message("-- mcbre > WDK @ ${MCBRE_WDK_PATH}/${MCBRE_WDK_PLATFORM}/Include/${MCBRE_WDK_VERSION}")
endif() # MCBRE_DONT_CARE_WIN_DEPS

# Setup build type macro definition
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message("-- mcbre > MCBRE_DEBUG_BUILD=1")
  add_compile_definitions(MCBRE_DEBUG_BUILD=1)
else()
  message("-- mcbre > MCBRE_DEBUG_BUILD=0")
  add_compile_definitions(MCBRE_DEBUG_BUILD=0)
endif()

# Head Project
project(
  mcbre
)

# Dependencies
add_subdirectory("deps/minhook")

# Projects
add_subdirectory("sdk/")
add_subdirectory("common/")
add_subdirectory("loader/")
add_subdirectory("client/")
