cmake_minimum_required(VERSION 3.10)
project(alflib)

# Compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
if (WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif ()

# Project headers
include_directories(
  .
)

# Project sources
set(COMMON_SOURCE
  alf_collection.c
  alf_test.c
  alf_thread.c
  alf_unicode.c
  tests/test.c
  )

# Executable
add_executable(${PROJECT_NAME} ${COMMON_SOURCE})

# Set the working directory for the Visual Studio debugger
if (WIN32)
set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()