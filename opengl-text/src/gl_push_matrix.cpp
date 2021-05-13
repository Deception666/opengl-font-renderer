#include "gl_push_matrix.h"
#include "gl_validate.h"

namespace opengl {
namespace gl {

PushMatrix::PushMatrix(
   const GLenum mode ) noexcept :
mode_ { mode }
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (mode_ == GL_PROJECTION)
   {
      glMatrixMode(
         mode);
   }

   glPushMatrix();

   if (mode_ == GL_PROJECTION)
   {
      glMatrixMode(
         GL_MODELVIEW);
   }

   VALIDATE_NO_GL_ERROR();
}

PushMatrix::~PushMatrix( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (mode_ == GL_PROJECTION)
   {
      glMatrixMode(
         mode_);
   }

   glPopMatrix();

   if (mode_ == GL_PROJECTION)
   {
      glMatrixMode(
         GL_MODELVIEW);
   }

   VALIDATE_NO_GL_ERROR();
}

}} // namespace opengl
