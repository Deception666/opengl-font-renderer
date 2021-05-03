#include "gl_bind_texture.h"
#include "gl_validate.h"

namespace opengl {
namespace gl {

BindTexutre::BindTexutre(
   const GLenum target,
   const GLuint texture_id ) noexcept :
target_ { target },
texture_id_ { texture_id }
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glBindTexture(
      target_,
      texture_id_);

   VALIDATE_NO_GL_ERROR();
}

BindTexutre::~BindTexutre() noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glBindTexture(
      target_,
      0);

   VALIDATE_NO_GL_ERROR();
}

}} // namespace opengl
