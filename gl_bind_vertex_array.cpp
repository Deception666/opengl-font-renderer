#include "gl_bind_vertex_array.h"
#include "gl_vertex_array.h"

namespace opengl {
namespace gl {

BindVertexArray::BindVertexArray(
   VertexArray * const vertex_array ) noexcept :
vertex_array_ { vertex_array }
{
   if (vertex_array_)
   {
      vertex_array_->Activate();
   }
}

BindVertexArray::~BindVertexArray() noexcept
{
   if (vertex_array_)
   {
      vertex_array_->Deactivate();
   }
}

}} // namespace opengl::gl
