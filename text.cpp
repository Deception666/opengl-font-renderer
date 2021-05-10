#include "text.h"
#include "error_reporting_private.h"
#include "font_engine.h"
#include "font_engine_factory.h"
#include "font_engine_type.h"
#include "font_texture_manager.h"
#include "gl_bind_shader_program.h"
#include "gl_bind_texture.h"
#include "gl_bind_vertex_array.h"
#include "gl_defines.h"
#include "gl_enable_blend.h"
#include "gl_enable_state.h"
#include "gl_extensions.h"
#include "gl_includes.h"
#include "gl_push_matrix.h"
#include "gl_shader.h"
#include "gl_shader_program.h"
#include "gl_validate.h"
#include "gl_vertex_array.h"
#include "gl_vertex_buffer.h"

#include <exception>
#include <stdexcept>
#include <vector>

namespace opengl
{

Text::Text( ) noexcept :
#if _WIN32
Text { "calibri.ttf" }
#else
#error "Define for this platform!"
#endif
{
}

Text::Text(
   const std::string & font_filename ) noexcept :
Text { font_filename, 48 }
{
}

Text::Text(
   const std::string & font_filename,
   const uint32_t size ) noexcept :
Text { font_filename, size, FontEngineType::FREETYPE }
{
}

Text::Text(
   const std::string & font_filename,
   const uint32_t size,
   const FontEngineType font_engine_type ) noexcept :
font_engine_ {
   font_engine_factory::ConstructFontEngine(
      font_filename,
      size,
      font_engine_type) },
update_ { true },
release_texture_ { false },
scale_ { 1.0f },
position_ { },
gl_data_ { false, GLData { } }
{
}

bool Text::Render( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (!gl_data_.first)
   {
      gl_data_.second =
         InitializeGLData();
   
      gl_data_.first = true;
   }

   if (update_)
   {
      RegenerateVertices();
   }

   const bool rendered =
      RenderText();

   VALIDATE_NO_GL_ERROR();

   return
      rendered;
}

bool Text::SetFont(
   const std::string & font_filename ) noexcept
{
   bool set { false };

   if (font_engine_)
   {
      set =
         font_engine_->SetFont(
            font_filename);

      update_ =
         update_ || set;

      release_texture_ =
         release_texture_ || set;
   }

   return set;
}

bool Text::SetFont(
   const std::string & font_filename,
   const uint32_t size ) noexcept
{
   bool set { false };

   if (font_engine_)
   {
      set =
         font_engine_->SetFont(
            font_filename,
            size);

      update_ =
         update_ || set;

      release_texture_ =
         release_texture_ || set;
   }

   return set;
}

std::string Text::GetFont( ) const noexcept
{
   return
      font_engine_ ?
      font_engine_->GetFont() :
      std::string { };
}

bool Text::SetFontSize(
   const uint32_t size ) noexcept
{
   bool set { false };

   if (font_engine_)
   {
      set =
         font_engine_->SetSize(
            size);

      update_ =
         update_ || set;

      release_texture_ =
         release_texture_ || set;
   }

   return set;
}

uint32_t Text::GetFontSize( ) const noexcept
{
   return
      font_engine_ ?
      font_engine_->GetSize() :
      0u;
}

uint32_t Text::GetFontMaxWidth( ) const noexcept
{
   return
      font_engine_ ?
      font_engine_->GetGlyphMaxWidth() :
      0u;
}

uint32_t Text::GetFontMaxHeight( ) const noexcept
{
   return
      font_engine_ ?
      font_engine_->GetGlyphMaxHeight() :
      0u;
}

int32_t Text::GetFontMaxTop() const noexcept
{
   return
      font_engine_ ?
      font_engine_->GetGlyphMaxTop() :
      0u;
}

bool Text::SetPosition(
   const float x,
   const float y,
   const float z ) noexcept
{
   position_[0] = x;
   position_[1] = y;
   position_[2] = z;

   return true;
}

const float (&Text::GetPosition( ) const noexcept)[3]
{
   return
      position_;
}

bool Text::SetScale(
   const float scale ) noexcept
{
   scale_ = scale;

   return true;
}

float Text::GetScale( ) const noexcept
{
   return
      scale_;
}

bool Text::PrependChar(
   const char c ) noexcept
{
   update_ = true;

   text_.insert(
      text_.cbegin(),
      c);

   return true;
}

bool Text::PrependText(
   const std::string & text ) noexcept
{
   return
      PrependText(
         text.c_str());
}

bool Text::PrependText(
   const char * const text ) noexcept
{
   bool prepended { false };

   if (text && *text)
   {
      text_.insert(
         0,
         text);

      update_ = prepended = true;
   }

   return
      prepended;
}

bool Text::AppendChar(
   const char c ) noexcept
{
   update_ = true;

   text_.append(
      1,
      c);

   return true;
}

bool Text::AppendText(
   const std::string & text ) noexcept
{
   return
      AppendText(
         text.c_str());
}

bool Text::AppendText(
   const char * const text ) noexcept
{
   bool appended { false };

   if (text && *text)
   {
      text_.append(
         text);

      update_ = appended = true;
   }

   return
      appended;
}

bool Text::SetText(
   std::string text ) noexcept
{
   bool set { false };

   if (text != text_)
   {
      text_.swap(
         text);

      update_ = set = true;
   }

   return set;
}

bool Text::SetText(
   const char * const text ) noexcept
{
   bool set { false };

   if (text && text_ != text)
   {
      text_.assign(
         text);

      update_ = set = true;
   }

   return set;
}

const std::string & Text::GetText( ) const noexcept
{
   return
      text_;
}

size_t Text::GetTextSize( ) const noexcept
{
   return
      text_.size();
}

static void GenerateVertices(
   const float x_left,
   const float y_bottom,
   const FontEngine::Metric & glyph_metric,
   std::vector< float > & verts_tex_coords ) noexcept
{
   verts_tex_coords.emplace_back(x_left);
   verts_tex_coords.emplace_back(y_bottom + glyph_metric.height);
   verts_tex_coords.emplace_back(0.0f);
   verts_tex_coords.emplace_back(
      glyph_metric.tex_coords.normalized.left);
   verts_tex_coords.emplace_back(
      glyph_metric.tex_coords.normalized.top);

   verts_tex_coords.emplace_back(x_left);
   verts_tex_coords.emplace_back(y_bottom);
   verts_tex_coords.emplace_back(0.0f);
   verts_tex_coords.emplace_back(
      glyph_metric.tex_coords.normalized.left);
   verts_tex_coords.emplace_back(
      glyph_metric.tex_coords.normalized.bottom);

   verts_tex_coords.emplace_back(x_left + glyph_metric.width);
   verts_tex_coords.emplace_back(y_bottom);
   verts_tex_coords.emplace_back(0.0f);
   verts_tex_coords.emplace_back(
      glyph_metric.tex_coords.normalized.right);
   verts_tex_coords.emplace_back(
      glyph_metric.tex_coords.normalized.bottom);

   verts_tex_coords.emplace_back(x_left);
   verts_tex_coords.emplace_back(y_bottom + glyph_metric.height);
   verts_tex_coords.emplace_back(0.0f);
   verts_tex_coords.emplace_back(
      glyph_metric.tex_coords.normalized.left);
   verts_tex_coords.emplace_back(
      glyph_metric.tex_coords.normalized.top);

   verts_tex_coords.emplace_back(x_left + glyph_metric.width);
   verts_tex_coords.emplace_back(y_bottom);
   verts_tex_coords.emplace_back(0.0f);
   verts_tex_coords.emplace_back(
      glyph_metric.tex_coords.normalized.right);
   verts_tex_coords.emplace_back(
      glyph_metric.tex_coords.normalized.bottom);

   verts_tex_coords.emplace_back(x_left + glyph_metric.width);
   verts_tex_coords.emplace_back(y_bottom + glyph_metric.height);
   verts_tex_coords.emplace_back(0.0f);
   verts_tex_coords.emplace_back(
      glyph_metric.tex_coords.normalized.right);
   verts_tex_coords.emplace_back(
      glyph_metric.tex_coords.normalized.top);
}

void Text::RegenerateVertices( ) noexcept
{
   if (font_engine_ &&
       gl_data_.second.vertex_buffer_)
   {
      if (release_texture_)
      {
         font_texture_.reset();

         release_texture_ = false;
      }

      font_engine_texture_ =
         font_engine_->GetGlyphTextureMap().texture_map;

      std::vector< float > verts_tex_coords;

      float pen_x { };
      float pen_y { };

      for (const auto c : text_)
      {
         if (c == '\n')
         {
            pen_x = 0.0f;
            pen_y -=
               static_cast< float >(
                  font_engine_->GetVerticalAdvance());
         }
         else
         {
            const auto glyph =
               font_engine_->GetGlyphMetric(
                  c);

            if (glyph)
            {
               if (c != ' ')
               {
                  const float x_left =
                     pen_x + glyph->left;
                  const float y_bottom =
                     pen_y - (static_cast< float >(glyph->height) - glyph->top);

                  GenerateVertices(
                     x_left,
                     y_bottom,
                     *glyph,
                     verts_tex_coords);
               }

               pen_x += glyph->advance;
            }
         }
      }

      if (font_engine_texture_.expired() ||
          !font_texture_)
      {
         RegenerateTexture();
      }

      const auto data =
         !verts_tex_coords.empty() ?
         verts_tex_coords.data() :
         nullptr;

      gl_data_.second.vertex_buffer_->SetData(
         data,
         verts_tex_coords.size());
   }

   update_ = false;
}

void Text::RegenerateTexture( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   const bool font_texture_is_nullptr =
      !font_texture_;

   if (font_texture_is_nullptr && font_engine_)
   {
      const auto font_texture_map =
         font_engine_->GetGlyphTextureMap();

      font_texture_ =
         font_texture_manager::CreateTexture(
            font_texture_map.width,
            font_texture_map.height,
            font_engine_->GetFont(),
            font_engine_->GetSize());

      VALIDATE_NO_GL_ERROR();
   }

   const bool requires_update =
      font_texture_is_nullptr &&
      font_texture_.use_count() == 1 ||
      font_engine_texture_.expired();

   if (font_texture_ && font_engine_ && requires_update)
   {
      const auto font_texture_map =
         font_engine_->GetGlyphTextureMap();

      font_texture_manager::UpdateTexture(
         font_texture_,
         font_texture_map.width,
         font_texture_map.height,
         font_texture_map.texture_map.lock());

      font_engine_texture_ =
         font_texture_map.texture_map;

      VALIDATE_NO_GL_ERROR();
   }
}

bool Text::RenderText( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   const bool render =
      gl_data_.second.shader_program_ &&
      gl_data_.second.vertex_array_ &&
      gl_data_.second.vertex_buffer_ &&
      gl_data_.second.vertex_buffer_->GetSize() &&
      font_texture_ && *font_texture_;

   if (render)
   {
      const gl::BindTexutre bind_texture {
         GL_TEXTURE_2D,
         *font_texture_,
         GL_TEXTURE0_ARB };

      const gl::EnableState enable_tex_2d {
         GL_TEXTURE_2D };

      const gl::EnableBlend enable_blend {
         GL_SRC_ALPHA,
         GL_SRC_ALPHA,
         GL_ONE_MINUS_SRC_ALPHA,
         GL_ONE_MINUS_SRC_ALPHA };

      const gl::PushMatrix push_matrix {
         GL_MODELVIEW };

      glTranslatef(
         position_[0],
         position_[1],
         position_[2]);
      glScalef(
         scale_,
         scale_,
         scale_);

      const gl::BindShaderProgram bind_shader_program {
         gl_data_.second.shader_program_.get() };

      const gl::BindVertexArray bind_vertex_array {
         gl_data_.second.vertex_array_.get() };

      const auto vertex_count =
         gl_data_.second.vertex_buffer_->GetSize< float >() / 5;

      glDrawArrays(
         GL_TRIANGLES,
         0,
         vertex_count);
   }

   VALIDATE_NO_GL_ERROR();

   return
      render;
}

std::unique_ptr< gl::ShaderProgram >
CreateShaderProgram( )
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   gl::Shader vertex_shader {
      GL_VERTEX_SHADER_ARB
   };

   auto error =
      vertex_shader.SetSource(
         R"(#version 420 compatibility
            layout (location = 0) in vec3 position;
            layout (location = 1) in vec2 tex_coord;

            out VS_OUT
            {
               smooth vec2 tex_coord;
            } fragment_shader_in;

            void main( )
            {
               // gl_ModelViewProjectionMatrix is only able to be
               // used in compatibility mode of the gl context.
               // we also need to define compatibility of the shader
               // to be able to pull in compatibility features.
               gl_Position =
                  gl_ModelViewProjectionMatrix *
                  vec4(position, 1.0f);

               fragment_shader_in.tex_coord =
                  tex_coord;
            }
         )");

   if (!error.empty())
   {
      throw
         std::runtime_error {
            "Unable to create vertex shader: " + error
         };
   }

   gl::Shader fragment_shader {
      GL_FRAGMENT_SHADER_ARB
   };

   error =
      fragment_shader.SetSource(
         R"(#version 420 compatibility
            layout (binding = 0) uniform sampler2D glyph_texture;

            layout (location = 0) out vec4 frag_color;

            in VS_OUT
            {
               smooth vec2 tex_coord;
            } fragment_shader_in;
            
            void main( )
            {
               float tex_value =
                  texture(
                     glyph_texture,
                     fragment_shader_in.tex_coord).r;

               if (tex_value == 0.0f)
                  discard;

               frag_color =
                  vec4(1.0f, 1.0f, 1.0f, tex_value);
            }
         )");

   if (!error.empty())
   {
      throw
         std::runtime_error {
            "Unable to create fragment shader: " + error
         };
   }

   auto shader_program =
      std::make_unique< gl::ShaderProgram >();

   shader_program->AttachShader(
      vertex_shader);
   shader_program->AttachShader(
      fragment_shader);

   error =
      shader_program->LinkProgram();

   if (!error.empty())
   {
      throw
         std::runtime_error {
            "Unable to create shader program: " + error
         };
   }

   VALIDATE_NO_GL_ERROR();

   return
      shader_program;
}

std::unique_ptr< gl::VertexArray >
CreateVertexArray(
   const gl::VertexBuffer & vertex_buffer )
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   auto vertex_array =
      std::make_unique< gl::VertexArray >();

   vertex_array->EnableVertexAttribute(
      0);
   vertex_array->BindVertexBuffer(
      vertex_buffer,
      0,
      0,
      5 * sizeof(float));
   vertex_array->BindVertexAttribute(
      0,
      0,
      3,
      GL_FLOAT,
      GL_FALSE,
      0);

   vertex_array->EnableVertexAttribute(
      1);
   vertex_array->BindVertexAttribute(
      0,
      1,
      2,
      GL_FLOAT,
      GL_FALSE,
      3 * sizeof(float));

   VALIDATE_NO_GL_ERROR();

   return
      vertex_array;
}

void ValidateOpenGL( )
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   GLint version_major { };
   GLint version_minor { };

   glGetIntegerv(
      GL_MAJOR_VERSION,
      &version_major);
   glGetIntegerv(
      GL_MINOR_VERSION,
      &version_minor);

   const uint32_t MIN_GL_VERSION {
      4u << 16 | 5u
   };

   const uint32_t gl_version {
      static_cast< uint32_t >(version_major) << 16 |
      static_cast< uint32_t >(version_minor)
   };

   if (MIN_GL_VERSION > gl_version)
   {
      const bool dsa_supported =
         gl::ExtensionIsSupported(
            "GL_ARB_direct_state_access");

      if (!dsa_supported)
      {
         throw
            std::runtime_error {
               "OpenGL version must be 4.5 or greater or "
               "the context must support direct state access."
            };
      }
   }

   VALIDATE_NO_GL_ERROR();
}

Text::GLData Text::InitializeGLData( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   GLData data;

   try
   {
      ValidateOpenGL();

      auto shader_program =
         CreateShaderProgram();

      auto vertex_buffer =
         std::make_unique< gl::VertexBuffer >(
            GL_DYNAMIC_DRAW_ARB);

      vertex_buffer->SetData(
         nullptr,
         0);

      auto vertex_array =
         CreateVertexArray(
            *vertex_buffer);

      data.shader_program_.swap(
         shader_program);
      data.vertex_buffer_.swap(
         vertex_buffer);
      data.vertex_array_.swap(
         vertex_array);
   }
   catch (const std::exception & e)
   {
      ReportError(
         e.what());
   }

   VALIDATE_NO_GL_ERROR();

   return data;
}

} // namespace opengl
