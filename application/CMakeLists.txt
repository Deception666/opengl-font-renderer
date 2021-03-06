cmake_minimum_required(
   VERSION 3.19.4)

set(
   proj_name
   opengl-text-test)

add_executable(
   ${proj_name}
   main.cpp
   scene.h
   qt.conf
   word_processor_scene.cpp
   word_processor_scene.h)

if (MSVC)
   target_compile_definitions(
      ${proj_name}
      PRIVATE
      NOMINMAX
      WIN32_LEAN_AND_MEAN)

   target_compile_options(
      ${proj_name}
      PRIVATE
      /Zi)

   target_link_options(
      ${proj_name}
      PRIVATE
      /DEBUG:FULL)
endif ( )

set_target_properties(
   ${proj_name}
   PROPERTIES
   DEBUG_POSTFIX "-d"
   AUTOMOC yes)

find_package(
   Qt5
   REQUIRED
   COMPONENTS
   Core
   Gui
   Widgets)
find_package(
   OpenGL
   REQUIRED)

target_link_libraries(
   ${proj_name}
   PRIVATE
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   OpenGL::GL
   opengl::text)

install(
   TARGETS ${proj_name}
   RUNTIME
      DESTINATION opengl-text/bin)
