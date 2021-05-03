#include "gl_enable_state.h"
#include "gl_validate.h"

namespace opengl {
namespace gl {

EnableState::EnableState(
   const GLenum state ) noexcept :
state_ { state }
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glEnable(
      state_);

   VALIDATE_NO_GL_ERROR();
}

EnableState::~EnableState( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glDisable(
      state_);

   VALIDATE_NO_GL_ERROR();
}

}} // namespace opengl
