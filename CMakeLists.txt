cmake_minimum_required(
   VERSION 3.19.4)

project(
   modern-font-render)

set(
   CMAKE_CXX_STANDARD
   17)

if (WIN32)
   add_compile_options(
      /MP
      /permissive-)
endif ( )

add_executable(
   font-render
   font_engine.h
   font_engine_default.cpp
   font_engine_default.h
   font_engine_default_data_windows.h
   font_engine_factory.cpp
   font_engine_factory.h
   font_engine_freetype.cpp
   font_engine_freetype.h
   freetype.cpp
   freetype.h
   main.cpp
   qt.conf)

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
   font-render
   PRIVATE
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   OpenGL::GL)
