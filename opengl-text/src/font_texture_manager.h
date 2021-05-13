#ifndef _OPENGL_FONT_TEXTURE_MANAGER_H_
#define _OPENGL_FONT_TEXTURE_MANAGER_H_

#include <cstdint>
#include <memory>
#include <string>

namespace opengl {
namespace font_texture_manager {

std::shared_ptr< uint32_t >
CreateTexture(
   const uint32_t width,
   const uint32_t heigth,
   const std::string & font_filename,
   const uint32_t font_size ) noexcept;

void UpdateTexture(
   const std::shared_ptr< uint32_t > & texture,
   const uint32_t width,
   const uint32_t height,
   const std::shared_ptr< const uint8_t [] > & pixels ) noexcept;

}} // namespace opengl::font_texture_manager

#endif // _OPENGL_FONT_TEXTURE_MANAGER_H_
