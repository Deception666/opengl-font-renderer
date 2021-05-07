#include "text.h"
#include "font_engine.h"
#include "font_engine_factory.h"
#include "font_engine_type.h"
#include "font_texture_manager.h"
#include "gl_bind_texture.h"
#include "gl_enable_client_state.h"
#include "gl_enable_state.h"
#include "gl_includes.h"
#include "gl_push_matrix.h"
#include "gl_validate.h"

#include <utility>

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
position_ { }
{
}

bool Text::IsValid( ) const noexcept
{
   return
      font_engine_ != nullptr;
}

bool Text::Render( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (update_)
   {
      RegenerateVertices();
   }

   RenderText();

   VALIDATE_NO_GL_ERROR();

   return true;
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
   std::vector< float > & vertices,
   std::vector< float > & texture_coords ) noexcept
{
   texture_coords.emplace_back(
      glyph_metric.tex_coords.normalized.left);
   texture_coords.emplace_back(
      glyph_metric.tex_coords.normalized.top);
   vertices.emplace_back(x_left);
   vertices.emplace_back(y_bottom + glyph_metric.height);
   vertices.emplace_back(0.0f);

   texture_coords.emplace_back(
      glyph_metric.tex_coords.normalized.left);
   texture_coords.emplace_back(
      glyph_metric.tex_coords.normalized.bottom);
   vertices.emplace_back(x_left);
   vertices.emplace_back(y_bottom);
   vertices.emplace_back(0.0f);

   texture_coords.emplace_back(
      glyph_metric.tex_coords.normalized.right);
   texture_coords.emplace_back(
      glyph_metric.tex_coords.normalized.bottom);
   vertices.emplace_back(x_left + glyph_metric.width);
   vertices.emplace_back(y_bottom);
   vertices.emplace_back(0.0f);

   texture_coords.emplace_back(
      glyph_metric.tex_coords.normalized.left);
   texture_coords.emplace_back(
      glyph_metric.tex_coords.normalized.top);
   vertices.emplace_back(x_left);
   vertices.emplace_back(y_bottom + glyph_metric.height);
   vertices.emplace_back(0.0f);

   texture_coords.emplace_back(
      glyph_metric.tex_coords.normalized.right);
   texture_coords.emplace_back(
      glyph_metric.tex_coords.normalized.bottom);
   vertices.emplace_back(x_left + glyph_metric.width);
   vertices.emplace_back(y_bottom);
   vertices.emplace_back(0.0f);

   texture_coords.emplace_back(
      glyph_metric.tex_coords.normalized.right);
   texture_coords.emplace_back(
      glyph_metric.tex_coords.normalized.top);
   vertices.emplace_back(x_left + glyph_metric.width);
   vertices.emplace_back(y_bottom + glyph_metric.height);
   vertices.emplace_back(0.0f);
}

void Text::RegenerateVertices( ) noexcept
{
   if (font_engine_)
   {
      if (release_texture_)
      {
         font_texture_.reset();

         release_texture_ = false;
      }

      font_engine_texture_ =
         font_engine_->GetGlyphTextureMap().texture_map;

      vertices_.clear();
      texture_coords_.clear();

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
                     vertices_,
                     texture_coords_);
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

void Text::RenderText( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (!vertices_.empty() &&
       !texture_coords_.empty() &&
       font_texture_ && *font_texture_)
   {
      const gl::EnableClientState vertex_array {
         GL_VERTEX_ARRAY };
      const gl::EnableClientState tex_coord_array {
         GL_TEXTURE_COORD_ARRAY };

      const gl::BindTexutre bind_texture {
         GL_TEXTURE_2D,
         *font_texture_ };

      const gl::EnableState enable_tex_2d {
         GL_TEXTURE_2D };

      const gl::PushMatrix push_matrix {
         GL_MODELVIEW };

      glVertexPointer(
         3,
         GL_FLOAT,
         0,
         vertices_.data());
      glTexCoordPointer(
         2,
         GL_FLOAT,
         0,
         texture_coords_.data());

      glTranslatef(
         position_[0],
         position_[1],
         position_[2]);
      glScalef(
         scale_,
         scale_,
         scale_);

      glDrawArrays(
         GL_TRIANGLES,
         0,
         vertices_.size() / 3);
   }

   VALIDATE_NO_GL_ERROR();
}

} // namespace opengl
