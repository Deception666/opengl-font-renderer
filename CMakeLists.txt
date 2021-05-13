cmake_minimum_required(
   VERSION 3.19.4)

project(
   modern-font-render)

set(
   CMAKE_CONFIGURATION_TYPES
   Debug
   Release)

set(
   CMAKE_CXX_STANDARD
   17)

if (WIN32)
   add_compile_options(
      /MP
      /permissive-
      /W4
      /DNOMINMAX
      /DWIN32_LEAN_AND_MEAN)
endif ( )

add_subdirectory(opengl-text)
add_subdirectory(application)
