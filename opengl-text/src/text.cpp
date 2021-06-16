#include "text.h"
#include "error_reporting_private.h"
#include "font_engine.h"
#include "font_engine_factory.h"
#include "font_engine_type.h"
#include "font_texture_manager.h"
#include "gl_bind_shader_program.h"
#include "gl_bind_texture.h"
#include "gl_bind_uniform_buffer.h"
#include "gl_bind_vertex_array.h"
#include "gl_defines.h"
#include "gl_enable_blend.h"
#include "gl_enable_state.h"
#include "gl_extensions.h"
#include "gl_includes.h"
#include "gl_shader.h"
#include "gl_shader_program.h"
#include "gl_uniform_buffer.h"
#include "gl_validate.h"
#include "gl_vertex_array.h"
#include "gl_vertex_buffer.h"
#include "text_uniform_data.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <exception>
#include <new>
#include <stdexcept>

namespace opengl
{

static void RegenerateVertices(
   const std::string & text,
   const float scale_factor,
   FontEngine * const font_engine,
   std::vector< float > & verts_tex_coords,
   BoundingBox & bounding_box ) noexcept;

std::pair< float, float >
CalculateOffset(
   const Text::AlignFlags align_flags,
   const BoundingBox & bounding_box ) noexcept;

Text::GLData::GLData( ) noexcept = default;
Text::GLData::~GLData( ) noexcept = default;
Text::GLData::GLData( GLData && ) noexcept = default;
Text::GLData & Text::GLData::operator = ( GLData && ) noexcept = default;

Text::Text( ) noexcept :
#if _WIN32
Text { "calibri.ttf" }
#elif __linux__
Text { "DejaVuSans.ttf" }
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
regenerate_vertices_ { true },
release_texture_ { false },
align_flags_ { Align::TOP | Align::LEFT },
gl_data_ { false, GLData { } }
{
   static_assert(
      sizeof(GLData::gl_uniform_data_) == sizeof(TextUniformData),
      "Increase gl_uniform_data_ size to match TextUniformData.");

   new (gl_data_.second.gl_uniform_data_)
      TextUniformData { true };
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

   if (regenerate_vertices_)
   {
      RegenerateVertices();
   }

   if (GetUniformData().update_)
   {
      UpdateUniformData();
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
         font_engine_->GetFont() != font_filename &&
         font_engine_->SetFont(
            font_filename);

      regenerate_vertices_ =
         regenerate_vertices_ || set;

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
         (font_engine_->GetFont() != font_filename ||
         font_engine_->GetSize() != size) &&
         font_engine_->SetFont(
            font_filename,
            size);

      regenerate_vertices_ =
         regenerate_vertices_ || set;

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
         font_engine_->GetSize() != size &&
         font_engine_->SetSize(
            size);

      regenerate_vertices_ =
         regenerate_vertices_ || set;

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
   auto & data =
      GetUniformData();

   data.layout_.position_[0] = x;
   data.layout_.position_[1] = y;
   data.layout_.position_[2] = z;

   data.update_ = true;

   return true;
}

const float (&Text::GetPosition( ) const noexcept)[3]
{
   return
      GetUniformData().layout_.position_;
}

bool Text::SetColor(
   const float r,
   const float g,
   const float b ) noexcept
{
   auto & data =
      GetUniformData();

   data.layout_.color_[0] =
      std::max(0.0f, std::min(1.0f, r));
   data.layout_.color_[1] =
      std::max(0.0f, std::min(1.0f, g));
   data.layout_.color_[2] =
      std::max(0.0f, std::min(1.0f, b));

   data.update_ = true;

   return true;
}

const float (&Text::GetColor( ) const noexcept)[3]
{
   return
      GetUniformData().layout_.color_;
}

bool Text::SetScale(
   const float scale ) noexcept
{
   auto & data =
      GetUniformData();

   if (scale != data.layout_.scale_)
   {
      const float scale_percentage =
         (scale - data.layout_.scale_) /
         data.layout_.scale_;

      bounding_box_.Scale(
         1 + scale_percentage);
   }

   data.layout_.scale_ = scale;

   data.update_ = true;

   return true;
}

float Text::GetScale( ) const noexcept
{
   return
      GetUniformData().layout_.scale_;
}

bool Text::SetAlignment(
   const AlignFlags flags ) noexcept
{
   assert(((flags & 0x07) & ((flags & 0x07) - 1)) == 0);
   assert(((flags & 0x70) & ((flags & 0x70) - 1)) == 0);

   align_flags_ = flags;

   return true;
}

Text::AlignFlags Text::GetAlignment( ) const noexcept
{
   return
      align_flags_;
}

BoundingBox Text::GetBoundingBox( ) noexcept
{
   // always call either before rendering
   // or after rendering to make sure the
   // attributes are as up-to-date.

   if (regenerate_vertices_)
   {
      opengl::RegenerateVertices(
         text_,
         GetScale(),
         font_engine_.get(),
         gl_data_.second.vertex_buffer_data_,
         bounding_box_);
   }

   const auto offset =
      CalculateOffset(
         align_flags_,
         bounding_box_);
   
   const auto & position =
      GetPosition();

   return
      bounding_box_.Translate({
         position[0] + offset.first,
         position[1] + offset.second,
         position[2] });
}

bool Text::PrependChar(
   const char c ) noexcept
{
   regenerate_vertices_ = true;

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

      regenerate_vertices_ =
         prepended = true;
   }

   return
      prepended;
}

bool Text::AppendChar(
   const char c ) noexcept
{
   regenerate_vertices_ = true;

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

      regenerate_vertices_ =
         appended = true;
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

      regenerate_vertices_ =
         set = true;
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

      regenerate_vertices_ =
         set = true;
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

static void GenerateVertexData(
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

static void ExpandBoundingBox(
   BoundingBox & bounding_box,
   const float current_pen_x,
   const float current_pen_y,
   const float scale_factor,
   const FontEngine::Metric & metric ) noexcept
{
   const float next_pen_x =
      current_pen_x + static_cast< float >(metric.advance);
   const float right =
      current_pen_x + metric.left + metric.width;
   const float left =
      current_pen_x + metric.left;
   const float top =
      current_pen_y + metric.top;
   const float bottom =
      top - metric.height;

   bounding_box.Expand({
      scale_factor * left,
      scale_factor * top,
      0.0f });
   bounding_box.Expand({
      scale_factor * right,
      scale_factor * bottom,
      0.0f });
}

static void RegenerateVertices(
   const std::string & text,
   const float scale_factor,
   FontEngine * const font_engine,
   std::vector< float > & verts_tex_coords,
   BoundingBox & bounding_box ) noexcept
{
   verts_tex_coords.clear();

   if (font_engine)
   {
      bounding_box =
         !text.empty() ?
         BoundingBox::InitInvalid() :
         BoundingBox { };

      float pen_x { };
      float pen_y { };

      for (const auto c : text)
      {
         if (c == '\n')
         {
            pen_x = 0.0f;
            pen_y -=
               static_cast< float >(
                  font_engine->GetVerticalAdvance());
         }
         else
         {
            const auto glyph =
               font_engine->GetGlyphMetric(
                  c);

            if (glyph)
            {
               if (c != ' ')
               {
                  const float x_left =
                     pen_x + glyph->left;
                  const float y_bottom =
                     pen_y - (static_cast< float >(glyph->height) - glyph->top);

                  GenerateVertexData(
                     x_left,
                     y_bottom,
                     *glyph,
                     verts_tex_coords);
               }

               ExpandBoundingBox(
                  bounding_box,
                  pen_x,
                  pen_y,
                  scale_factor,
                  *glyph);

               pen_x +=
                  static_cast< float >(
                     glyph->advance);
            }
         }
      }
   }
}

void Text::RegenerateVertices( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (font_engine_ &&
       gl_data_.second.vertex_buffer_)
   {
      if (release_texture_)
      {
         font_texture_.reset();

         release_texture_ = false;
      }

      if (gl_data_.second.vertex_buffer_data_.empty())
      {
         font_engine_texture_ =
            font_engine_->GetGlyphTextureMap().texture_map;

         opengl::RegenerateVertices(
            text_,
            GetScale(),
            font_engine_.get(),
            gl_data_.second.vertex_buffer_data_,
            bounding_box_);
      }

      if (font_engine_texture_.expired() ||
          !font_texture_)
      {
         RegenerateTexture();
      }

      const auto data =
         !gl_data_.second.vertex_buffer_data_.empty() ?
         gl_data_.second.vertex_buffer_data_.data() :
         nullptr;

      gl_data_.second.vertex_buffer_->SetData(
         data,
         gl_data_.second.vertex_buffer_data_.size());

      gl_data_.second.vertex_buffer_data_.clear();
   }

   regenerate_vertices_ = false;

   VALIDATE_NO_GL_ERROR();
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
      gl_data_.second.uniform_buffer_ &&
      gl_data_.second.vertex_buffer_ &&
      gl_data_.second.vertex_buffer_->GetSize() &&
      font_texture_ && *font_texture_;

   if (render)
   {
      UpdateOffset();

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

      const gl::BindUniformBuffer bind_ubo {
         gl_data_.second.uniform_buffer_.get(),
         gl_data_.second.shader_program_.get(),
         "TEXT_DATA" };

      const gl::BindShaderProgram bind_shader_program {
         gl_data_.second.shader_program_.get() };

      const gl::BindVertexArray bind_vertex_array {
         gl_data_.second.vertex_array_.get() };

      const auto vertex_count =
         static_cast< GLsizei >(
            gl_data_.second.vertex_buffer_->GetSize< float >() / 5);

      glDrawArrays(
         GL_TRIANGLES,
         0,
         vertex_count);
   }

   VALIDATE_NO_GL_ERROR();

   return
      render;
}

static std::unique_ptr< gl::ShaderProgram >
CreateShaderProgram( )
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   gl::Shader vertex_shader {
      GL_VERTEX_SHADER_ARB
   };

   auto error =
      vertex_shader.SetSource(
         R"(#version 420 compatibility
            layout (location = 0) in vec3 vertex;
            layout (location = 1) in vec2 tex_coord;

            layout (std140) uniform TEXT_DATA
            {
               float scale;
               vec3 position;
               vec3 offset;
               vec3 color;
            } text_data;

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
                  vec4((text_data.position.xyz + text_data.offset) +
                       vertex.xyz * text_data.scale,
                       1.0f);

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

            layout (std140) uniform TEXT_DATA
            {
               float scale;
               vec3 position;
               vec3 offset;
               vec3 color;
            } text_data;

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
                  vec4(text_data.color, tex_value);
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

static std::unique_ptr< gl::VertexArray >
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

static void ValidateOpenGL( )
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

      auto uniform_buffer =
         std::make_unique< gl::UniformBuffer >(
            GL_STREAM_DRAW_ARB);

      data.shader_program_.swap(
         shader_program);
      data.vertex_buffer_.swap(
         vertex_buffer);
      data.vertex_array_.swap(
         vertex_array);
      data.uniform_buffer_.swap(
         uniform_buffer);

      std::copy(
         gl_data_.second.gl_uniform_data_,
         gl_data_.second.gl_uniform_data_ +
         std::size(gl_data_.second.gl_uniform_data_),
         data.gl_uniform_data_);
   }
   catch (const std::exception & e)
   {
      ReportError(
         e.what());
   }

   VALIDATE_NO_GL_ERROR();

   return data;
}

TextUniformData & Text::GetUniformData( ) noexcept
{
   return
      reinterpret_cast< TextUniformData & >(
         gl_data_.second.gl_uniform_data_);
}

const TextUniformData & Text::GetUniformData( ) const noexcept
{
   return
      const_cast< Text * >(this)->GetUniformData();
}

void Text::UpdateUniformData( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (gl_data_.second.uniform_buffer_)
   {
      auto & uniform_data =
         GetUniformData();

      gl_data_.second.uniform_buffer_->SetData(
         &uniform_data.layout_,
         sizeof(TextUniformData::Layout));

      uniform_data.update_ = false;
   }

   VALIDATE_NO_GL_ERROR();
}

std::pair< float, float >
CalculateOffset(
   const Text::AlignFlags align_flags,
   const BoundingBox & bounding_box ) noexcept
{
   float x_offset { };

   switch (align_flags & 0x07)
   {
   case Text::Align::HCENTER:
      x_offset = -bounding_box.GetWidth() / 2.0f;
      break;

   case Text::Align::RIGHT:
      x_offset = -bounding_box.GetWidth();
      break;

   case Text::Align::LEFT:
   default:
      x_offset = 0.0f;
      break;
   }

   float y_offset { };

   switch (align_flags & 0x70)
   {
   case Text::Align::VCENTER:
      y_offset =
         bounding_box.GetHeight() / 2.0f -
         std::get< 1 >(bounding_box.GetUpperLeftFront());
      break;

   case Text::Align::BOTTOM:
      y_offset =
         bounding_box.GetHeight() -
         std::get< 1 >(bounding_box.GetUpperLeftFront());
      break;

   case Text::Align::TOP:
   default:
      y_offset =
         -std::get< 1 >(bounding_box.GetUpperLeftFront());
      break;
   }

   return {
      x_offset,
      y_offset
   };
}

void Text::UpdateOffset( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   const auto offset =
      CalculateOffset(
         align_flags_,
         bounding_box_);

   auto & uniform_data =
      GetUniformData();

   if (offset.first != uniform_data.layout_.offset_[0] ||
       offset.second != uniform_data.layout_.offset_[1])
   {
      uniform_data.layout_.offset_[0] = offset.first;
      uniform_data.layout_.offset_[1] = offset.second;

      gl_data_.second.uniform_buffer_->SetData(
         &uniform_data.layout_.offset_,
         offsetof(TextUniformData::Layout, offset_),
         sizeof(TextUniformData::Layout::offset_));
   }

   VALIDATE_NO_GL_ERROR();
}

} // namespace opengl
