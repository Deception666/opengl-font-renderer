#include "font_engine_default.h"

namespace opengl
{

FontEngineDefault::~FontEngineDefault( ) noexcept
{
}

bool FontEngineDefault::Initialize( ) noexcept
{
   return false;
}

bool FontEngineDefault::SetFont(
   const std::string & font_filename ) noexcept
{
    return false;
}

bool FontEngineDefault::SetFont(
   const std::string & font_filename,
   const uint32_t size ) noexcept
{
    return false;
}

std::string FontEngineDefault::GetFont( ) const noexcept
{
    static std::string s;
    return s;
}

bool FontEngineDefault::SetSize(
   const uint32_t size ) noexcept
{
    return false;
}

uint32_t FontEngineDefault::GetSize( ) const noexcept
{
    return uint32_t();
}

uint32_t FontEngineDefault::GetGlyphMaxWidth( ) const noexcept
{
   return uint32_t();
}

uint32_t FontEngineDefault::GetGlyphMaxHeight( ) const noexcept
{
   return uint32_t();
}

int32_t FontEngineDefault::GetGlyphMaxTop( ) const noexcept
{
   return int32_t();
}

uint32_t FontEngineDefault::GetLineHeight( ) const noexcept
{
   return uint32_t();
}

const FontEngine::Metric * FontEngineDefault::GetGlyphMetric(
   const uint32_t character ) noexcept
{
    return nullptr;
}

FontEngine::TextureMap FontEngineDefault::GetGlyphTextureMap( ) noexcept
{
    return TextureMap();
}

} // namespace opengl
