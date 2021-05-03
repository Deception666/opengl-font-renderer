#include "gl_enable_client_state.h"
#include "gl_validate.h"

namespace opengl {
namespace gl {

EnableClientState::EnableClientState(
   const GLenum state ) noexcept :
state_ { state }
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glEnableClientState(
      state_);

   VALIDATE_NO_GL_ERROR();
}

EnableClientState::~EnableClientState( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glDisableClientState(
      state_);

   VALIDATE_NO_GL_ERROR();
}

}} // namespace opengl
