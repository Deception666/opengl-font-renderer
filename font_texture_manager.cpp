#include "font_texture_manager.h"
#include "calling_convention.h"
#include "gl_defines.h"
#include "gl_includes.h"
#include "gl_validate.h"
#include "reverse_lock.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <mutex>
#include <new>
#include <sstream>
#include <thread>
#include <unordered_map>

namespace opengl {
namespace font_texture_manager {

std::unordered_map<
   std::string,
   std::weak_ptr< uint32_t > >
   font_textures_;

std::mutex
   font_textures_mutex_;

void ReleaseTexture(
   const uint32_t * const texture ) noexcept
{
   if (texture && *texture)
   {
      VALIDATE_ACTIVE_GL_CONTEXT();

      glDeleteTextures(
         1,
         texture);

      VALIDATE_NO_GL_ERROR();
   }

   delete texture;
}

void AllocateTextureStorage(
   const uint32_t texture,
   const uint32_t width,
   const uint32_t height ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

#if _WIN32
   const auto glTexStorage2D =
      reinterpret_cast< void (CALL_CONV *) ( GLenum, GLsizei, GLenum, GLsizei, GLsizei ) >(
         wglGetProcAddress("glTexStorage2D"));
   const auto glGenerateMipmap =
      wglGetProcAddress("glGenerateMipmap");
#else
#error "Define for this platform!"
#endif

   glBindTexture(
      GL_TEXTURE_2D,
      texture);

   if (!glTexStorage2D)
   {
      glTexImage2D(
         GL_TEXTURE_2D,
         0,
         GL_LUMINANCE,
         width,
         height,
         0,
         GL_RED,
         GL_UNSIGNED_BYTE,
         nullptr);
   }
   else
   {
      const int32_t max_levels =
         glGenerateMipmap ?
         1 +
         static_cast< int32_t >(
            std::floor(std::log2(std::max(width, height)))) :
         1;

      glTexStorage2D(
         GL_TEXTURE_2D,
         max_levels,
         GL_LUMINANCE,
         width,
         height);
   }

   glTexParameteri(
      GL_TEXTURE_2D,
      GL_TEXTURE_WRAP_S,
      GL_CLAMP_TO_BORDER);
   glTexParameteri(
      GL_TEXTURE_2D,
      GL_TEXTURE_WRAP_T,
      GL_CLAMP_TO_BORDER);

   glTexParameteri(
      GL_TEXTURE_2D,
      GL_TEXTURE_MIN_FILTER,
      glGenerateMipmap ?
      GL_LINEAR_MIPMAP_LINEAR :
      GL_LINEAR);
   glTexParameteri(
      GL_TEXTURE_2D,
      GL_TEXTURE_MAG_FILTER,
      GL_LINEAR);

   glBindTexture(
      GL_TEXTURE_2D,
      0);

   VALIDATE_NO_GL_ERROR();
}

std::shared_ptr< uint32_t >
CreateTexture(
   std::unique_lock< std::mutex > & lock,
   const uint32_t width,
   const uint32_t height ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   reverse_lock rlock {
      *lock.mutex() };

   std::shared_ptr< uint32_t > texture {
      new (std::nothrow) uint32_t { },
      &ReleaseTexture
   };

   if (texture)
   {
      glGenTextures(
         1,
         texture.get());

      if (*texture)
      {
         AllocateTextureStorage(
            *texture,
            width,
            height);
      }
   }

   VALIDATE_NO_GL_ERROR();

   return
      texture;
}

std::shared_ptr< uint32_t >
CreateTexture(
   const uint32_t width,
   const uint32_t height,
   const std::string & font_filename,
   const uint32_t font_size ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   std::shared_ptr< uint32_t >
      font_texture;

   std::stringstream ss_index;

   ss_index
      << std::this_thread::get_id() << " "
      << font_filename << " "
      << font_size << " "
      << width << " "
      << height;

   const std::string index {
      ss_index.str() };

   std::unique_lock lock {
      font_textures_mutex_ };

   auto font_texture_it =
      font_textures_.find(
         index);

   if (font_texture_it == font_textures_.cend() ||
       font_texture_it->second.expired())
   {
      font_texture =
         CreateTexture(
            lock,
            width,
            height);

      font_texture_it =
         font_textures_.emplace(
            index,
            std::weak_ptr< uint32_t > { }).first;

      font_texture_it->second =
         std::weak_ptr< uint32_t > {
            font_texture };
   }
   else
   {
      font_texture =
         font_texture_it->second.lock();
   }

   VALIDATE_NO_GL_ERROR();

   return
      font_texture;
}

void UpdateTexture(
   const std::shared_ptr< uint32_t > & texture,
   const uint32_t width,
   const uint32_t height,
   const std::shared_ptr< const uint8_t [] > & pixels ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

#if _WIN32
   const auto glGenerateMipmap =
      reinterpret_cast< void (CALL_CONV *) ( GLenum ) >(
         wglGetProcAddress("glGenerateMipmap"));
#else
#error "Define for this platform!"
#endif

   if (texture && *texture && pixels)
   {
      glBindTexture(
         GL_TEXTURE_2D,
         *texture);

      glPixelStorei(
         GL_UNPACK_ALIGNMENT,
         1);

      glTexSubImage2D(
         GL_TEXTURE_2D,
         0,
         0, 0,
         width,
         height,
         GL_RED,
         GL_UNSIGNED_BYTE,
         pixels.get());

      if (glGenerateMipmap)
      {
         glGenerateMipmap(
            GL_TEXTURE_2D);
      }

      glPixelStorei(
         GL_UNPACK_ALIGNMENT,
         4);

      glBindTexture(
         GL_TEXTURE_2D,
         0);
   }

   VALIDATE_NO_GL_ERROR();
}

}} // namespace opengl::font_texture_manager
