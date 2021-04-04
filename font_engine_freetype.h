#ifndef _OPENGL_FONT_ENGINE_FREETYPE_H_
#define _OPENGL_FONT_ENGINE_FREETYPE_H_

#include "font_engine.h"

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <utility>

namespace opengl
{

class FreeType;

class FontEngineFreeType final :
   public FontEngine
{
public:
   FontEngineFreeType( ) noexcept;
   virtual ~FontEngineFreeType( ) noexcept;

   virtual bool Initialize( ) noexcept override;

   virtual bool SetFont(
      const std::string & font_filename ) noexcept override;
   virtual bool SetFont(
      const std::string & font_filename,
      const uint32_t size ) noexcept override;
   virtual std::string GetFont( ) const noexcept override;

   virtual bool SetSize(
      const uint32_t size ) noexcept override;
   virtual uint32_t GetSize( ) const noexcept override;

   virtual uint32_t GetGlyphMaxWidth( ) const noexcept override;
   virtual uint32_t GetGlyphMaxHeight( ) const noexcept override;
   virtual int32_t GetGlyphMaxTop( ) const noexcept override;
   virtual uint32_t GetLineHeight( ) const noexcept override;

   virtual const Metric * GetGlyphMetric(
      const uint32_t character ) noexcept override;

   virtual TextureMap GetGlyphTextureMap( ) noexcept override;

private:
   struct TextureData
   {
      std::pair< uint32_t, uint32_t >
         texture_map_size;
      std::shared_ptr< uint8_t [] >
         texture_map;
   };

   struct MetricData
   {
      uint32_t glyph_max_height { };
      uint32_t glyph_max_width { };
      int32_t glyph_max_top { };
      uint32_t line_height { };

      uint32_t current_row { };
      uint32_t current_column { };
      uint32_t next_row { };

      std::unordered_map< uint32_t, Metric >
         metrics;
   };

   TextureCoords GenerateTextureCoords(
      const uint32_t glyph_width,
      const uint32_t glyph_height,
      const int32_t glyph_top,
      MetricData & metric_data ) const noexcept;

   void RegenerateGlyphData( ) noexcept;

   Metric * LoadGlyphMetric(
      const uint32_t character ) noexcept;

   MetricData GenerateGlyphMetrics( ) const noexcept;

   void RegenerateTextureData( ) noexcept;

   std::pair<
      std::pair< uint32_t, uint32_t >,
      std::shared_ptr< uint8_t [] > >
   GenerateTextureMap( ) const noexcept;

   TextureData texture_data_;

   MetricData metric_data_;

   std::shared_ptr< FreeType >
      freetype_;

};

} // namespace opengl

#endif // _OPENGL_FONT_ENGINE_FREETYPE_H_
