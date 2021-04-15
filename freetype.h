#ifndef _OPENGL_FREETYPE_H_
#define _OPENGL_FREETYPE_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace opengl
{

using FT_Module =
   std::unique_ptr< void, void (*) ( void * const ) >;
using FT_Instance =
   std::unique_ptr< void, int32_t (*) ( void * const ) >;
using FT_Face =
   std::unique_ptr< void, int32_t (*) ( void * const ) >;

class FreeType final
{
public:
   static std::shared_ptr< FreeType >
   Create( ) noexcept;

   bool SetFont(
      const std::string & font_filename ) noexcept;
   const std::string & GetFont( ) const noexcept;

   bool SetSize(
      const uint32_t size ) noexcept;
   uint32_t GetSize( ) const noexcept;

   struct Glyph
   {
      uint32_t width;
      uint32_t height;
      int32_t top;
      int32_t left;
      double advance;
      // bitmap data; origin upper left
      std::vector< uint8_t > bitmap;
   };

   const Glyph * GetGlyph(
      const uint32_t character ) noexcept;

   double GetGlobalGlyphHeight( ) const noexcept;

private:
   FreeType(
      FT_Module module );

   bool SetSize(
      const FT_Face::pointer face,
      const uint32_t size ) noexcept;

   uint32_t GetBitmapWidth(
      const FT_Face::pointer face ) const noexcept;
   uint32_t GetBitmapHeight(
      const FT_Face::pointer face ) const noexcept;
   const uint8_t * GetBitmapData(
      const FT_Face::pointer face ) const noexcept;
   int32_t GetGlyphTop(
      const FT_Face::pointer face ) const noexcept;
   int32_t GetGlyphLeft(
      const FT_Face::pointer face ) const noexcept;
   double GetGlyphAdvance(
      const FT_Face::pointer face ) const noexcept;
   uint16_t GetFaceUnitsPerEM(
      const FT_Face::pointer face ) const noexcept;
   int16_t GetFaceAscender(
      const FT_Face::pointer face ) const noexcept;
   int16_t GetFaceDescender(
      const FT_Face::pointer face ) const noexcept;

   int32_t (* const init_) ( FT_Instance::pointer * );
   int32_t (* const uninit_) ( FT_Instance::pointer );
   void (* const version_) ( FT_Instance::pointer, int32_t *, int32_t *, int32_t * );
   int32_t (* const new_face_) ( FT_Instance::pointer, const char *, int32_t, FT_Face::pointer * );
   int32_t (* const delete_face_) ( FT_Face::pointer );
   int32_t (* const set_pixel_sizes_) ( FT_Face::pointer, uint32_t, uint32_t );
   int32_t (* const load_char_) ( FT_Face::pointer, uint32_t, int32_t );

   uint32_t size_;
   std::string font_filename_;
   std::unordered_map< uint32_t, Glyph >
      glyphs_;

   FT_Module module_;
   FT_Instance instance_;
   FT_Face face_;

};

} // namespace opengl

#endif // _OPENGL_FREETYPE_H_
