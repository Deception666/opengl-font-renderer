#ifndef _OPENGL_GL_BIND_SHADER_PROGRAM_H_
#define _OPENGL_GL_BIND_SHADER_PROGRAM_H_

namespace opengl {
namespace gl {

class ShaderProgram;

class BindShaderProgram final
{
public:
   BindShaderProgram(
      ShaderProgram * const shader_program ) noexcept;
   ~BindShaderProgram( ) noexcept;

private:
   ShaderProgram * const shader_program_;
   
};

}} // namespace opengl

#endif // _OPENGL_GL_BIND_SHADER_PROGRAM_H_
