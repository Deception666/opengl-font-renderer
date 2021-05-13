cmake_minimum_required(
   VERSION 3.19.4)

set(
   source
   src/calling_convention.h
   src/error_reporting_private.cpp
   src/error_reporting_private.h
   src/font_engine.h
   src/font_engine_default.cpp
   src/font_engine_default.h
   src/font_engine_default_data_windows.h
   src/font_engine_factory.cpp
   src/font_engine_factory.h
   src/font_engine_freetype.cpp
   src/font_engine_freetype.h
   src/font_engine_type.h
   src/font_texture_manager.cpp
   src/font_texture_manager.h
   src/freetype.cpp
   src/freetype.h
   src/gl_bind_shader_program.cpp
   src/gl_bind_shader_program.h
   src/gl_bind_texture.cpp
   src/gl_bind_texture.h
   src/gl_bind_uniform_buffer.cpp
   src/gl_bind_uniform_buffer.h
   src/gl_bind_vertex_array.cpp
   src/gl_bind_vertex_array.h
   src/gl_defines.h
   src/gl_enable_blend.cpp
   src/gl_enable_blend.h
   src/gl_enable_client_state.cpp
   src/gl_enable_client_state.h
   src/gl_enable_state.cpp
   src/gl_enable_state.h
   src/gl_extensions.cpp
   src/gl_extensions.h
   src/gl_get_proc_address.h
   src/gl_includes.h
   src/gl_push_matrix.cpp
   src/gl_push_matrix.h
   src/gl_shader.cpp
   src/gl_shader.h
   src/gl_shader_program.cpp
   src/gl_shader_program.h
   src/gl_uniform_buffer.cpp
   src/gl_uniform_buffer.h
   src/gl_validate.h
   src/gl_vertex_array.cpp
   src/gl_vertex_array.h
   src/gl_vertex_buffer.cpp
   src/gl_vertex_buffer.h
   src/reverse_lock.h
   src/text.cpp
   src/text_uniform_data.h)

set(
   includes
   opengl-text/error_reporting.h
   opengl-text/text.h)

add_library(
   opengl-text
   STATIC
   ${source}
   ${includes})

add_library(
   opengl-text::text
   ALIAS
   opengl-text)

find_package(
   OpenGL
   REQUIRED)

target_include_directories(
   opengl-text
   PUBLIC
   .
   PRIVATE
   ./opengl-text)

target_link_libraries(
   opengl-text
   PRIVATE
   OpenGL::GL)
