cmake_minimum_required(
   VERSION 3.19.4)

add_executable(
   opengl-text-test
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
   opengl-text-test
   PRIVATE
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   OpenGL::GL
   opengl-text::text)
