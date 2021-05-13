#ifndef _OPENGL_TEXT_UNIFORM_DATA_H_
#define _OPENGL_TEXT_UNIFORM_DATA_H_

#include <cstdint>

namespace opengl
{

struct TextUniformData
{
   bool update_ { false };

   struct Layout
   {
      float scale_ { 1.0f };
      uint32_t padding_1[3] { };
      float position_[3] { };
      uint32_t padding_2 { };
      float color_[3] { 1.0f, 1.0f, 1.0f };
      uint32_t padding_3 { };
   } layout_;
};

} // namespace opengl

#endif // _OPENGL_TEXT_UNIFORM_DATA_H_
