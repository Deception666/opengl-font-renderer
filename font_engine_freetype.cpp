#include "font_engine_freetype.h"
#include "freetype.h"

#include <algorithm>
#include <new>
#include <string>

namespace opengl
{

static const uint32_t TEXTURE_WIDTH { 2048 };
static const uint32_t TEXTURE_HEIGHT { 2048 };

FontEngineFreeType::FontEngineFreeType( ) noexcept
{
}

FontEngineFreeType::~FontEngineFreeType( ) noexcept
{
}

bool FontEngineFreeType::Initialize( ) noexcept
{
   freetype_ =
      FreeType::Create();

   return
      freetype_ != nullptr;
}

bool FontEngineFreeType::SetFont(
   const std::string & font_filename ) noexcept
{
   const bool set {
      freetype_ &&
      freetype_->SetFont(
         font_filename)
   };

   if (set)
   {
      RegenerateGlyphData();
   }

   return set;
}

bool FontEngineFreeType::SetFont(
   const std::string & font_filename,
   const uint32_t size ) noexcept
{
   bool set { false };

   if (freetype_)
   {
      const auto old_font_filename =
         freetype_->GetFont();
      const auto old_font_size =
         freetype_->GetSize();

      set =
         freetype_->SetFont(font_filename) &&
         freetype_->SetSize(size);

      if (!set)
      {
         freetype_->SetFont(
            old_font_filename);
         freetype_->SetSize(
            old_font_size);
      }
      else
      {
         RegenerateGlyphData();
      }
   }

   return set;
}

std::string FontEngineFreeType::GetFont( ) const noexcept
{
   return
      freetype_ ?
      freetype_->GetFont() :
      std::string { };
}

bool FontEngineFreeType::SetSize(
   const uint32_t size ) noexcept
{
   const bool set {
      freetype_ &&
      freetype_->SetSize(size)
   };

   if (set)
   {
      RegenerateGlyphData();
   }

   return set;
}

uint32_t FontEngineFreeType::GetSize( ) const noexcept
{
   return
      freetype_ ?
      freetype_->GetSize() :
      0;
}

uint32_t FontEngineFreeType::GetGlyphMaxWidth( ) const noexcept
{
   return
      metric_data_.glyph_max_width;
}

uint32_t FontEngineFreeType::GetGlyphMaxHeight( ) const noexcept
{
   return
      metric_data_.glyph_max_height;
}

int32_t FontEngineFreeType::GetGlyphMaxTop( ) const noexcept
{
   return
      metric_data_.glyph_max_top;
}

uint32_t FontEngineFreeType::GetLineHeight( ) const noexcept
{
   return
      metric_data_.line_height;
}

const FontEngine::Metric * FontEngineFreeType::GetGlyphMetric(
   const uint32_t character ) noexcept
{
   const Metric * metric { nullptr };

   if (freetype_)
   {
      const decltype(MetricData::metrics)::const_iterator loaded_metric =
         metric_data_.metrics.find(
            character);

      if (loaded_metric !=
          metric_data_.metrics.cend())
      {
         metric =
            &loaded_metric->second;
      }
      else
      {
         metric =
            LoadGlyphMetric(
               character);
      }
   }

   return
      metric;
}

FontEngine::TextureMap
FontEngineFreeType::GetGlyphTextureMap( ) noexcept
{
   TextureMap texture_map { };

   if (freetype_)
   {
      if (texture_data_.texture_map)
      {
         texture_map.width = texture_data_.texture_map_size.first;
         texture_map.height = texture_data_.texture_map_size.second;

         texture_map.texture_map =
            decltype(TextureData::texture_map)::weak_type {
               texture_data_.texture_map };
      }
      else
      {
         auto temp_texture_map =
            GenerateTextureMap();

         if (temp_texture_map.second)
         {
            texture_data_.texture_map_size =
               temp_texture_map.first;
            texture_data_.texture_map =
               temp_texture_map.second;

            texture_map =
               GetGlyphTextureMap();
         }
      }
   }

   return
      texture_map;
}

FontEngineFreeType::TextureCoords
FontEngineFreeType::GenerateTextureCoords(
   const uint32_t glyph_width,
   const uint32_t glyph_height,
   const int32_t glyph_top,
   MetricData & metric_data ) const noexcept
{
   TextureCoords tex_coords { };

   if (metric_data.current_column + glyph_width >= TEXTURE_WIDTH)
   {
      metric_data.current_column = 0;
      metric_data.current_row =
         metric_data.next_row;
   }

   if (metric_data.current_row + glyph_height < TEXTURE_HEIGHT)
   {
      tex_coords.absolute.left =
         metric_data.current_column;
      tex_coords.absolute.right =
         metric_data.current_column + glyph_width;
      tex_coords.absolute.bottom =
         metric_data.current_row;
      tex_coords.absolute.top =
         metric_data.current_row + glyph_height;

      tex_coords.normalized.left =
         tex_coords.absolute.left /
         static_cast< double >(TEXTURE_WIDTH - 1);
      tex_coords.normalized.right =
         tex_coords.absolute.right /
         static_cast< double >(TEXTURE_WIDTH - 1);
      tex_coords.normalized.top =
         tex_coords.absolute.top /
         static_cast< double >(TEXTURE_HEIGHT - 1);
      tex_coords.normalized.bottom =
         tex_coords.absolute.bottom /
         static_cast< double >(TEXTURE_HEIGHT - 1);

      metric_data.current_column +=
         glyph_width + 2;

      metric_data.next_row =
         std::max(
            metric_data.next_row,
            metric_data.current_row + glyph_height + 2);

      metric_data.glyph_max_height =
         std::max(
            glyph_height,
            metric_data.glyph_max_height);

      metric_data.glyph_max_width =
         std::max(
            glyph_width,
            metric_data.glyph_max_width);

      metric_data.glyph_max_top =
         std::max(
            glyph_top,
            metric_data.glyph_max_top);

      metric_data.line_height =
         std::max(
            metric_data.line_height,
            metric_data.glyph_max_height + glyph_height - glyph_top);
   }

   return
      tex_coords;
}

void FontEngineFreeType::RegenerateGlyphData( ) noexcept
{
   metric_data_ =
      GenerateGlyphMetrics();

   RegenerateTextureData();
}

FontEngine::Metric * FontEngineFreeType::LoadGlyphMetric(
   const uint32_t character ) noexcept
{
   Metric * metric { nullptr };

   if (freetype_)
   {
      const auto loaded_metric =
         metric_data_.metrics.find(
            character);

      if (loaded_metric ==
          metric_data_.metrics.end())
      {
         const auto glyph =
            freetype_->GetGlyph(
               character);

         if (glyph)
         {
            const auto inserted =
               metric_data_.metrics.emplace(
                  character,
                  Metric {
                     glyph->width,
                     glyph->height,
                     glyph->top,
                     glyph->left,
                     glyph->advance,
                     GenerateTextureCoords(
                        glyph->width,
                        glyph->height,
                        glyph->top,
                        metric_data_)
                  });

            if (inserted.second)
            {
               metric =
                  &inserted.first->second;

               RegenerateTextureData();
            }
         }
      }
   }

   return
      metric;
}

FontEngineFreeType::MetricData
FontEngineFreeType::GenerateGlyphMetrics( ) const noexcept
{
   MetricData metric_data;

   if (freetype_)
   {
      for (const auto & metric : metric_data_.metrics)
      {
         Metric new_metric { };

         const auto glyph =
            freetype_->GetGlyph(
               metric.first);

         if (glyph)
         {
            new_metric = {
               glyph->width,
               glyph->height,
               glyph->top,
               glyph->left,
               glyph->advance,
               GenerateTextureCoords(
                  glyph->width,
                  glyph->height,
                  glyph->top,
                  metric_data)
            };
         }

         metric_data.metrics.emplace(
            metric.first,
            std::move(new_metric));
      }
   }

   return
      metric_data;
}

void FontEngineFreeType::RegenerateTextureData( ) noexcept
{
   texture_data_ = { };
}

std::pair<
   std::pair< uint32_t, uint32_t >,
   std::shared_ptr< uint8_t [] > >
FontEngineFreeType::GenerateTextureMap( ) const noexcept
{
   decltype(GenerateTextureMap()) texture_map;

   if (freetype_)
   {
      const size_t tex_map_size {
         TEXTURE_WIDTH * TEXTURE_HEIGHT };

      texture_map.second =
         std::shared_ptr< uint8_t [] >(
            new (std::nothrow) uint8_t[tex_map_size] { });

      if (texture_map.second)
      {
         texture_map.first = {
            TEXTURE_WIDTH,
            TEXTURE_HEIGHT
         };

         for (const auto & metric : metric_data_.metrics)
         {
            const auto glyph =
               freetype_->GetGlyph(
                  metric.first);

            if (glyph)
            {
               const auto buffer =
                  glyph->bitmap.data();
               const auto glyph_width =
                  metric.second.width;
               const auto glyph_height =
                  metric.second.height;

               for (uint32_t i { }; glyph_height > i; ++i)
               {
                  std::copy(
                     buffer + glyph_width * (glyph_height - i - 1),
                     buffer + glyph_width * (glyph_height - i),
                     texture_map.second.get() +
                     (metric.second.tex_coords.absolute.bottom + i) * TEXTURE_WIDTH +
                     metric.second.tex_coords.absolute.left);
               }
            }
         }
      }
   }

   return
      texture_map;
}

} // namespace opengl
