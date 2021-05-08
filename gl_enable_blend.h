#ifndef _OPENGL_GL_ENABLE_BLEND_H_
#define _OPENGL_GL_ENABLE_BLEND_H_

#include "gl_includes.h"

#ifndef GL_BLEND_DST_RGB
#define GL_BLEND_DST_RGB      0x80C8
#endif

#ifndef GL_BLEND_SRC_RGB
#define GL_BLEND_SRC_RGB      0x80C9
#endif

#ifndef GL_BLEND_DST_ALPHA
#define GL_BLEND_DST_ALPHA    0x80CA
#endif

#ifndef GL_BLEND_SRC_ALPHA
#define GL_BLEND_SRC_ALPHA    0x80CB
#endif

namespace opengl {
namespace gl {

class EnableBlend final
{
public:
   EnableBlend(
      const GLenum source_rgb,
      const GLenum source_alpha,
      const GLenum destination_rgb,
      const GLenum destination_alpha ) noexcept;
   ~EnableBlend( ) noexcept;

private:
   const bool blend_was_enabled_;
   const GLenum current_source_rgb_;
   const GLenum current_source_alpha_;
   const GLenum current_destination_rgb_;
   const GLenum current_destination_alpha_;

};

}} // namespace opengl::gl

#endif // _OPENGL_GL_ENABLE_BLEND_H_
