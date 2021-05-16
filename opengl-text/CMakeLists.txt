cmake_minimum_required(
   VERSION 3.19.4)

set(
   proj_name
   opengl-text)

set(
   source
   src/bounding_box.cpp
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
   opengl-text/bounding_box.h
   opengl-text/error_reporting.h
   opengl-text/library_export.h
   opengl-text/text.h)

add_library(
   ${proj_name}
   ${source}
   ${includes})

add_library(
   opengl::text
   ALIAS
   ${proj_name})

set(
   debug_postfix
   -d)

if (MSVC)
   target_compile_definitions(
      ${proj_name}
      PUBLIC
      $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,OPENGL_TEXT_DLL,>
      PRIVATE
      NOMINMAX
      WIN32_LEAN_AND_MEAN)

   target_compile_options(
      ${proj_name}
      PRIVATE
      $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,/wd4251,>
      /Zi)

   target_link_options(
      ${proj_name}
      PRIVATE
      /DEBUG:FULL)
 
   if (NOT BUILD_SHARED_LIBS)
      set_target_properties(
         ${proj_name}
         PROPERTIES
         COMPILE_PDB_NAME_DEBUG ${proj_name}${debug_postfix}
         COMPILE_PDB_NAME_RELEASE ${proj_name})
   endif ( )
endif ( )

set_target_properties(
   ${proj_name}
   PROPERTIES
   DEBUG_POSTFIX "${debug_postfix}"
   PUBLIC_HEADER "${includes}"
   EXPORT_NAME text)

target_include_directories(
   ${proj_name}
   PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<INSTALL_INTERFACE:${proj_name}/include>
   PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/${proj_name})

find_package(
   OpenGL
   REQUIRED)

target_link_libraries(
   ${proj_name}
   PRIVATE
   OpenGL::GL)

install(
   TARGETS ${proj_name}
   EXPORT ${proj_name}-config
   RUNTIME
      DESTINATION ${proj_name}/bin
   LIBRARY
      DESTINATION ${proj_name}/lib
   ARCHIVE
      DESTINATION ${proj_name}/lib
   PUBLIC_HEADER
      DESTINATION ${proj_name}/include/${proj_name})

if (MSVC)
   if (BUILD_SHARED_LIBS)
      install(
         FILES
         $<TARGET_PDB_FILE:${proj_name}>
         DESTINATION ${proj_name}/bin)
   else ( )
      set(
         base_pdb_config
         $<TARGET_PROPERTY:${proj_name},ARCHIVE_OUTPUT_DIRECTORY>/$<CONFIG>)

      install(
         FILES
         $<IF:$<CONFIG:DEBUG>,${base_pdb_config}/$<TARGET_PROPERTY:${proj_name},COMPILE_PDB_NAME_DEBUG>.pdb,>
         $<IF:$<CONFIG:RELEASE>,${base_pdb_config}/$<TARGET_PROPERTY:${proj_name},COMPILE_PDB_NAME_RELEASE>.pdb,>
         DESTINATION ${proj_name}/lib)
   endif ( )
endif ( )

install(
   EXPORT ${proj_name}-config
   DESTINATION ${proj_name}/lib/${proj_name}
   NAMESPACE opengl::)
