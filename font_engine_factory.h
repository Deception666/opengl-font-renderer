#ifndef _OPENGL_FONT_ENGINE_FACTORY_H_
#define _OPENGL_FONT_ENGINE_FACTORY_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace opengl
{

class FontEngine;

namespace font_engine_factory
{

enum class FontEngineType : size_t
{
   DEFAULT,
   FREETYPE,
   MAX_TYPES
};

std::shared_ptr< FontEngine >
ConstructFontEngine(
   const std::string & font_filename,
   const uint32_t size,
   const FontEngineType type ) noexcept;

} // namespace font_engine_factory

} // namespace opengl

#endif // _OPENGL_FONT_ENGINE_FACTORY_H_
