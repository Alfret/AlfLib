cmake_minimum_required(VERSION 3.10)
project(alflib)

# Compiler flags
set(CMAKE_C_STANDARD 99)
if (WIN32)
  set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
else ()
  set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
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

# Link libraries
if (WIN32)
target_link_libraries(${PROJECT_NAME})
else()
target_link_libraries(${PROJECT_NAME} pthread)
endif()

# Set the working directory for the Visual Studio debugger
if (WIN32)
set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()