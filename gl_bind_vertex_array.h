#ifndef _OPENGL_GL_BIND_VERTEX_ARRAY_H_
#define _OPENGL_GL_BIND_VERTEX_ARRAY_H_

#include "gl_includes.h"

namespace opengl {
namespace gl {

class VertexArray;

class BindVertexArray final
{
public:
   BindVertexArray(
      VertexArray * const vertex_array ) noexcept;
   ~BindVertexArray( ) noexcept;

private:
   VertexArray * const vertex_array_;
   
};

}} // namespace opengl::gl

#endif // _OPENGL_GL_BIND_VERTEX_ARRAY_H_
