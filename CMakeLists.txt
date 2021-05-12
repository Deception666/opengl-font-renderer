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
      /permissive-
      /DNOMINMAX
      /DWIN32_LEAN_AND_MEAN)
endif ( )

add_executable(
   font-render
   calling_convention.h
   error_reporting.h
   error_reporting_private.cpp
   error_reporting_private.h
   font_engine.h
   font_engine_default.cpp
   font_engine_default.h
   font_engine_default_data_windows.h
   font_engine_factory.cpp
   font_engine_factory.h
   font_engine_freetype.cpp
   font_engine_freetype.h
   font_engine_type.h
   font_texture_manager.cpp
   font_texture_manager.h
   freetype.cpp
   freetype.h
   gl_bind_shader_program.cpp
   gl_bind_shader_program.h
   gl_bind_texture.cpp
   gl_bind_texture.h
   gl_bind_uniform_buffer.cpp
   gl_bind_uniform_buffer.h
   gl_bind_vertex_array.cpp
   gl_bind_vertex_array.h
   gl_defines.h
   gl_enable_blend.cpp
   gl_enable_blend.h
   gl_enable_client_state.cpp
   gl_enable_client_state.h
   gl_enable_state.cpp
   gl_enable_state.h
   gl_extensions.cpp
   gl_extensions.h
   gl_get_proc_address.h
   gl_includes.h
   gl_push_matrix.cpp
   gl_push_matrix.h
   gl_shader.cpp
   gl_shader.h
   gl_shader_program.cpp
   gl_shader_program.h
   gl_uniform_buffer.cpp
   gl_uniform_buffer.h
   gl_validate.h
   gl_vertex_array.cpp
   gl_vertex_array.h
   gl_vertex_buffer.cpp
   gl_vertex_buffer.h
   main.cpp
   qt.conf
   reverse_lock.h
   text.cpp
   text.h
   text_uniform_data.h)

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
