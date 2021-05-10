#ifndef _OPENGL_GL_BIND_UNIFORM_BUFFER_H_
#define _OPENGL_GL_BIND_UNIFORM_BUFFER_H_

namespace opengl {
namespace gl {

class UniformBuffer;
class ShaderProgram;

class BindUniformBuffer final
{
public:
   BindUniformBuffer(
      UniformBuffer * const uniform_buffer,
      const ShaderProgram * const shader_program,
      const char * const uniform_block_name ) noexcept;
   ~BindUniformBuffer( ) noexcept;

private:
   UniformBuffer * const uniform_buffer_;
   
};

}} // namespace opengl::gl

#endif // _OPENGL_GL_BIND_UNIFORM_BUFFER_H_
