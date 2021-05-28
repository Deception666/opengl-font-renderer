#include "font_engine_default.h"

#if _WIN32
#include "font_engine_default_data_windows.h"
#elif __linux__
#include "font_engine_default_data_linux.h"
#else
#error "Define for this platform type!"
#endif

#include <string>
#include <tuple>

namespace opengl
{

FontEngineDefault::~FontEngineDefault( ) noexcept
{
}

bool FontEngineDefault::Initialize( ) noexcept
{
   texture_map_.reset(
      DEFAULT_TEXTURE_MAP,
      [ ] ( const uint8_t * const ) { });

   return true;
}

bool FontEngineDefault::SetFont(
   const std::string & font_filename ) noexcept
{
   std::ignore = font_filename;

    return false;
}

bool FontEngineDefault::SetFont(
   const std::string & font_filename,
   const uint32_t size ) noexcept
{
   std::ignore = font_filename;
   std::ignore = size;

    return false;
}

std::string FontEngineDefault::GetFont( ) const noexcept
{
    return
       DEFAULT_FONT_FILENAME;
}

bool FontEngineDefault::SetSize(
   const uint32_t size ) noexcept
{
   std::ignore = size;

    return false;
}

uint32_t FontEngineDefault::GetSize( ) const noexcept
{
    return
       DEFAULT_GLYPH_FONT_PIXEL_SIZE;
}

uint32_t FontEngineDefault::GetGlyphMaxWidth( ) const noexcept
{
   return
      DEFAULT_GLYPH_MAX_WIDTH;
}

uint32_t FontEngineDefault::GetGlyphMaxHeight( ) const noexcept
{
   return
      DEFAULT_GLYPH_MAX_HEIGHT;
}

int32_t FontEngineDefault::GetGlyphMaxTop( ) const noexcept
{
   return
      DEFAULT_GLYPH_MAX_TOP;
}

double FontEngineDefault::GetVerticalAdvance( ) const noexcept
{
   return
      DEFAULT_VERTICAL_ADVANCE;
}

const FontEngine::Metric * FontEngineDefault::GetGlyphMetric(
   const uint32_t character ) noexcept
{
   const FontEngine::Metric * metric { nullptr };

   if (DEFAULT_GLYPH_METRICS_BEGIN <= character &&
       character < DEFAULT_GLYPH_METRICS_END)
   {
      const uint32_t index =
         character - DEFAULT_GLYPH_METRICS_BEGIN;

      metric =
         &DEFAULT_GLYPH_METRICS[index];
   }

   return metric;
}

FontEngine::TextureMap FontEngineDefault::GetGlyphTextureMap( ) noexcept
{
   return {
      DEFAULT_TEXTURE_WIDTH,
      DEFAULT_TEXTURE_HEIGHT,
      decltype(texture_map_)::weak_type {
         texture_map_
      }
   };
}

} // namespace opengl
