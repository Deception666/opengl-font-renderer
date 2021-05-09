#include "gl_enable_blend.h"
#include "error_reporting_private.h"
#include "calling_convention.h"
#include "gl_get_proc_address.h"
#include "gl_validate.h"

#include <mutex>

namespace opengl {
namespace gl {

bool IsBlendEnabled( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   GLboolean enabled { GL_FALSE };

   glGetBooleanv(
      GL_BLEND,
      &enabled);

   VALIDATE_NO_GL_ERROR();

   return
      GL_TRUE == enabled;
}

GLenum GetCurrentBlendState(
   const GLenum blend_state ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   GLint current_state { };

   glGetIntegerv(
      blend_state,
      &current_state);

   VALIDATE_NO_GL_ERROR();

   return
      current_state;
}

using glBlendFuncSeparateFuncPtr =
   void (CALL_CONV *) ( GLenum, GLenum, GLenum, GLenum );

glBlendFuncSeparateFuncPtr GetGLBlendFuncSeparate( ) noexcept
{
   static glBlendFuncSeparateFuncPtr
      glBlendFuncSeparate {
         nullptr
      };

   static std::once_flag
      get_glBlendFuncSeparate;

   std::call_once(
      get_glBlendFuncSeparate,
      [ ] ( )
      {
#if _WIN32
         glBlendFuncSeparate =
            GetProcAddress(
               "glBlendFuncSeparate",
               glBlendFuncSeparate);
#else
#error "Define for this platform!"
#endif

         if (!glBlendFuncSeparate)
         {
            ReportError(
               "Unable to obtain function pointer "
               "for glBlendFuncSeparate.  Blending "
               "operations may be compromised.");
         }
      });

   return glBlendFuncSeparate;
}

EnableBlend::EnableBlend(
   const GLenum source_rgb,
   const GLenum source_alpha,
   const GLenum destination_rgb,
   const GLenum destination_alpha ) noexcept :
blend_was_enabled_ { IsBlendEnabled() },
current_source_rgb_ { GetCurrentBlendState(GL_BLEND_SRC_RGB) },
current_source_alpha_ { GetCurrentBlendState(GL_BLEND_SRC_ALPHA) },
current_destination_rgb_ { GetCurrentBlendState(GL_BLEND_DST_RGB) },
current_destination_alpha_ { GetCurrentBlendState(GL_BLEND_DST_ALPHA) }
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (!blend_was_enabled_)
   {
      glEnable(
         GL_BLEND);
   }

   const auto glBlendFuncSeparate =
      GetGLBlendFuncSeparate();

   if (glBlendFuncSeparate)
   {
      glBlendFuncSeparate(
         source_rgb,
         destination_rgb,
         source_alpha,
         destination_alpha);
   }

   VALIDATE_NO_GL_ERROR();
}

EnableBlend::~EnableBlend( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (!blend_was_enabled_)
   {
      glDisable(
         GL_BLEND);
   }

   const auto glBlendFuncSeparate =
      GetGLBlendFuncSeparate();

   if (glBlendFuncSeparate)
   {
      glBlendFuncSeparate(
         current_source_rgb_,
         current_destination_rgb_,
         current_source_alpha_,
         current_destination_alpha_);
   }

   VALIDATE_NO_GL_ERROR();
}

}} // namesapce opengl::gl
