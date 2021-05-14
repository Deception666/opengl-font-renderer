cmake_minimum_required(
   VERSION 3.19.4)

option(
   BUILD_SHARED_LIBS
   "If present and true, this will cause all libraries to 
    be built shared unless the library was explicitly 
    added as a static library."
   no)

project(
   modern-font-render)

set(
   CMAKE_CONFIGURATION_TYPES
   Debug
   Release)

set(
   CMAKE_ARCHIVE_OUTPUT_DIRECTORY
   "${CMAKE_BINARY_DIR}/output")
set(
   CMAKE_RUNTIME_OUTPUT_DIRECTORY
   "${CMAKE_BINARY_DIR}/output")

set(
   CMAKE_CXX_STANDARD
   17)

if (WIN32)
   add_compile_options(
      /MP
      /permissive-
      /W4)
endif ( )

add_subdirectory(opengl-text)
add_subdirectory(application)
