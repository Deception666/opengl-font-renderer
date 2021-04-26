#ifndef _OPENGL_FONT_ENGINE_TYPE_H_
#define _OPENGL_FONT_ENGINE_TYPE_H_

#include <cstddef>

namespace opengl
{

enum class FontEngineType : size_t
{
   DEFAULT,
   FREETYPE,
   MAX_TYPES
};

} // namespace opengl

#endif // _OPENGL_FONT_ENGINE_TYPE_H_
