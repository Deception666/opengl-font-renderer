#ifndef _OPENGL_TEXT_H_
#define _OPENGL_TEXT_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace opengl
{

namespace gl
{
class ShaderProgram;
class VertexBuffer;
class VertexArray;
} // namespace gl

class FontEngine;

enum class FontEngineType : size_t;

class Text final
{
public:
   Text( ) noexcept;
   Text(
      const std::string & font_filename ) noexcept;
   Text(
      const std::string & font_filename,
      const uint32_t size ) noexcept;
   Text(
      const std::string & font_filename,
      const uint32_t size,
      const FontEngineType font_engine_type ) noexcept;

   bool Render( ) noexcept;

   bool SetFont(
      const std::string & font_filename ) noexcept;
   bool SetFont(
      const std::string & font_filename,
      const uint32_t size ) noexcept;
   std::string GetFont( ) const noexcept;

   bool SetFontSize(
      const uint32_t size ) noexcept;
   uint32_t GetFontSize( ) const noexcept;

   uint32_t GetFontMaxWidth( ) const noexcept;
   uint32_t GetFontMaxHeight( ) const noexcept;
   int32_t GetFontMaxTop( ) const noexcept;

   bool SetPosition( 
      const float x,
      const float y,
      const float z ) noexcept;
   const float (&GetPosition( ) const noexcept)[3];

   bool SetScale(
      const float scale ) noexcept;
   float GetScale( ) const noexcept;

   bool PrependChar(
      const char c ) noexcept;
   bool PrependText(
      const std::string & text ) noexcept;
   bool PrependText(
      const char * const text ) noexcept;
   bool AppendChar(
      const char c ) noexcept;
   bool AppendText(
      const std::string & text ) noexcept;
   bool AppendText(
      const char * const text ) noexcept;
   bool SetText(
      std::string text ) noexcept;
   bool SetText(
      const char * const text ) noexcept;
   const std::string & GetText( ) const noexcept;
   size_t GetTextSize( ) const noexcept;

private:
   struct GLData
   {
      std::unique_ptr< gl::ShaderProgram >
         shader_program_;
      std::unique_ptr< gl::VertexBuffer >
         vertex_buffer_;
      std::unique_ptr< gl::VertexArray >
         vertex_array_;
   };

   void RegenerateVertices( ) noexcept;
   void RegenerateTexture( ) noexcept;
   bool RenderText( ) noexcept;

   GLData InitializeGLData( ) noexcept;

   std::shared_ptr< FontEngine >
      font_engine_;
   std::weak_ptr< const uint8_t [] >
      font_engine_texture_;

   std::shared_ptr< uint32_t >
      font_texture_;

   bool update_;
   bool release_texture_;

   std::string text_;

   float scale_;
   float position_[3];

   std::pair< bool, GLData >
      gl_data_;

};

} // namespace opengl

#endif // _OPENGL_TEXT_H_
