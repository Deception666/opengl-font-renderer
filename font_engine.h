#ifndef _OPENGL_FONT_ENGINE_H_
#define _OPENGL_FONT_ENGINE_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace opengl
{

class FontEngine
{
public:
   virtual ~FontEngine( ) noexcept = 0 { }

   virtual bool Initialize( ) noexcept = 0;

   virtual bool SetFont(
      const std::string & font_filename ) noexcept = 0;
   virtual bool SetFont(
      const std::string & font_filename,
      const uint32_t size ) noexcept = 0;
   virtual std::string GetFont( ) const noexcept = 0;

   virtual bool SetSize(
      const uint32_t size ) noexcept = 0;
   virtual uint32_t GetSize( ) const noexcept = 0;

   virtual uint32_t GetGlyphMaxWidth( ) const noexcept = 0;
   virtual uint32_t GetGlyphMaxHeight( ) const noexcept = 0;
   virtual int32_t GetGlyphMaxTop( ) const noexcept = 0;
   virtual double GetVerticalAdvance( ) const noexcept = 0;

   struct TextureCoords
   {
      bool valid { false };

      struct Pixel
      {
         uint32_t top { };
         uint32_t bottom { };
         uint32_t left { };
         uint32_t right { };
      } absolute;

      struct Normalized
      {
         float top { };
         float bottom { };
         float left { };
         float right { };
      } normalized;
   };

   struct Metric
   {
      uint32_t width { };
      uint32_t height { };
      int32_t top { };
      int32_t left { };
      double advance { };
      TextureCoords tex_coords { };
   };

   virtual const Metric * GetGlyphMetric(
      const uint32_t character ) noexcept = 0;

   struct TextureMap
   {
      uint32_t width { };
      uint32_t height { };
      std::weak_ptr< const uint8_t [] > texture_map { };
   };

   virtual TextureMap GetGlyphTextureMap( ) noexcept = 0;

};

} // namespace opengl

#endif // _OPENGL_FONT_ENGINE_H_
