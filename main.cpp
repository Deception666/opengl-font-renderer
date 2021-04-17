#include <QtWidgets/QApplication>
#include <QtWidgets/QOpenGLWidget>

#include <cstdint>

//////////////////////////////////////////////////////////////////
#include <memory>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include <QtGui/QKeyEvent>
#include <QtGui/QImageWriter>

//#include <ft2build.h>
//#include <freetype/freetype.h>

#if _WIN32

#if _M_IX86
#define CALL_CONV __stdcall
#elif _M_X64
#define CALL_CONV
#else
#define "Define for this platform type!"
#endif

#endif // _WIN32

//#include "freetype.h"
#include "font_engine_factory.h"
#include "font_engine_freetype.h"
#include "font_engine_default.h"

//using FT_Module___ =
//   std::unique_ptr<
//      void,
//      int32_t (__stdcall *) ( void * ) >;
//
//using FT_Instance = void *;
//using FT_Face = void *;
//
//template < typename T >
//T GetFuncAddress(
//   const char * const function,
//   void * const module,
//   T ) noexcept
//{
//   return
//      reinterpret_cast< T >(
//         GetProcAddress(
//            reinterpret_cast< const HMODULE >(module),
//            function));
//}
//
//struct FreeTypeLibrary
//{
//   FreeTypeLibrary(
//      void * const module ) :
//   init { GetFuncAddress("FT_Init_FreeType", module, init) },
//   uninit { GetFuncAddress("FT_Done_FreeType", module, uninit) },
//   version { GetFuncAddress("FT_Library_Version", module, version) },
//   new_face { GetFuncAddress("FT_New_Face", module, new_face) },
//   delete_face { GetFuncAddress("FT_Done_Face", module, delete_face) },
//   set_pixel_sizes { GetFuncAddress("FT_Set_Pixel_Sizes", module, set_pixel_sizes) },
//   load_char { GetFuncAddress("FT_Load_Char", module, load_char) }
//   {
//   }
//
//   int32_t (* const init) ( FT_Instance * );
//   int32_t (* const uninit) ( FT_Instance );
//   void (* const version) ( FT_Instance, int32_t *, int32_t *, int32_t * );
//   int32_t (* const new_face) ( FT_Instance, const char *, long, FT_Face * );
//   int32_t (* const delete_face) ( FT_Face );
//   int32_t (* const set_pixel_sizes) ( FT_Face, uint32_t, uint32_t );
//   int32_t (* const load_char) ( FT_Face, uint32_t, int32_t );
//
//};
//
//template < size_t OFFSET, typename T >
//T GetBitmapData(
//   const FT_Face ft_face ) noexcept
//{
//#if _WIN64
//   const size_t glyph_offset { 120 };
//   const size_t bitmap_offset { 104 };
//#else
//#error "Define for this platform type!"
//#endif
//
//   const uint8_t * const glyph =
//      reinterpret_cast< const uint8_t * >(
//         *reinterpret_cast< const size_t * >(
//            reinterpret_cast< const uint8_t * >(
//               ft_face) + glyph_offset));
//
//   return
//      *reinterpret_cast< const T * >(
//         glyph + bitmap_offset + OFFSET);
//}
//
//template < size_t OFFSET, typename T >
//T GetGlyphData(
//   const FT_Face ft_face ) noexcept
//{
//#if _WIN64
//   const size_t glyph_offset { 120 };
//#else
//#error "Define for this platform type!"
//#endif
//
//   const uint8_t * const glyph =
//      reinterpret_cast< const uint8_t * >(
//         *reinterpret_cast< const size_t * >(
//            reinterpret_cast< const uint8_t * >(
//               ft_face) + glyph_offset));
//
//   return
//      *reinterpret_cast< const T * >(
//         glyph + OFFSET);
//}

float scale { 1.0f };
//int32_t glyph_max_top { -5000 };
//uint32_t glyph_max_height { 0 };
uint32_t size { 48 };
//uint32_t tex_width { 2048 }, tex_height { 2048 };
size_t font_index { 0 };
//std::unique_ptr< uint8_t [] > texture;

static std::string string;

//struct glyph
//{
//   uint32_t width;
//   uint32_t height;
//   int32_t top;
//   int32_t left;
//   double advance;
//   float uv_ll[2];
//   float uv_ur[2];
//};
//
//std::unique_ptr< glyph [] > glyphs;

std::shared_ptr< opengl::FontEngine >
   freetype_font_engine;
opengl::FontEngineFreeType::TextureMap
   font_engine_texture_map;
std::vector< uint32_t > default_char_set_;

void write_data( )
{
   std::ofstream o;

   o.open(
      "texture.txt",
      std::ios_base::out |
      std::ios_base::trunc);

   o
      << std::hex
      << std::uppercase
      << std::internal/*
      << std::setw(2)
      << std::setfill('0')*/;

   const auto texture_map =
      freetype_font_engine->GetGlyphTextureMap();
   
   const auto map =
      texture_map.texture_map.lock();

   for (uint32_t h { }; texture_map.height > h; ++h)
   {
      for (uint32_t r { }; texture_map.width > r; ++r)
      {
         o
            << "0x"
            << std::setw(2)
            << std::setfill('0')
            << static_cast< uint32_t >(
               *(map.get() + (texture_map.width * h + r)))
            << ", ";
      }

      o << "\n";
   }

   QImage image {
      static_cast< int32_t >(texture_map.width),
      static_cast< int32_t >(texture_map.height),
      QImage::Format::Format_Grayscale8
   };

   std::copy(
      map.get(),
      map.get() + texture_map.width * texture_map.height,
      image.bits());

   QImageWriter image_writer {
      "image.png"
   };

   image_writer.write(
      image);

   std::ofstream o2;

   o2.open(
      "metrics.txt",
      std::ios_base::out |
      std::ios_base::trunc);

   o2
      << "VA: " << freetype_font_engine->GetVerticalAdvance() << "\n"
      << "MH: " << freetype_font_engine->GetGlyphMaxHeight() << "\n"
      << "MW: " << freetype_font_engine->GetGlyphMaxWidth() << "\n"
      << "MT: " << freetype_font_engine->GetGlyphMaxTop() << "\n"
      << "SZ: " << freetype_font_engine->GetSize() << "\n\n";

   for (const auto c : default_char_set_)
   {
      const auto metric =
         freetype_font_engine->GetGlyphMetric(c);

      o2
         << "{ "
         << metric->width << ", "
         << metric->height << ", "
         << metric->top << ", "
         << metric->left << ", "
         << metric->advance << ", "
         << "{ { "
         << metric->tex_coords.absolute.top << ", "
         << metric->tex_coords.absolute.bottom << ", "
         << metric->tex_coords.absolute.left << ", "
         << metric->tex_coords.absolute.right
         << " }, "
         << "{ "
         << std::setprecision(10)
         << metric->tex_coords.normalized.top << "f, "
         << metric->tex_coords.normalized.bottom << "f, "
         << metric->tex_coords.normalized.left << "f, "
         << metric->tex_coords.normalized.right << "f"
         << " } } },\n";
   }
}

void font_test( )
{
   //const FT_Module___ module {
   //   LoadLibrary("freetyped.dll"),
   //   reinterpret_cast< int32_t (__stdcall *) ( void * ) >(&FreeLibrary)
   //};
   //
   //FreeTypeLibrary library {
   //   module.get()
   //};
   //
   //FT_Instance ft_instance { nullptr };
   //
   //library.init(
   //   &ft_instance);
   //
   //int32_t version[3] { };
   //
   //library.version(
   //   ft_instance,
   //   &version[0],
   //   &version[1],
   //   &version[2]);
   //
   //FT_Face ft_face { nullptr };

   //auto freetype =
   //   opengl::FreeType::Create();

   const char * const fonts[] {
      //"c:/windows/fonts/arial.ttf",
      //"c:/windows/fonts/itcedscr.ttf",
      //"c:/windows/fonts/bradhitc.ttf",
      //"c:/windows/fonts/consola.ttf"
      "arial.ttf",
      "itcedscr.ttf",
      "bradhitc.ttf",
      "consola.ttf",
      "times.ttf",
      "calibri.ttf",
      "cambria.ttc",
      "bkant.ttf"
   };

   size_t index =
      std::clamp(
         font_index,
         static_cast< size_t >(0),
         std::size(fonts) - 1);
   
   //static bool init { true };
   //if (!freetype_font_engine)
   {
      freetype_font_engine =
         opengl::font_engine_factory::ConstructFontEngine(
            fonts[index],
            size,
            opengl::font_engine_factory::FontEngineType::FREETYPE);

      std::string s =
         "scale: " + std::to_string(scale) + "\n"
         "size: " + std::to_string(::size) + "\n"
         + string;

      for (auto c : s)
      {
         freetype_font_engine->GetGlyphMetric(
            c);
      }
   }

   //freetype->SetFont(
   //   fonts[index]);
   //freetype_font_engine->SetFont(
   //   fonts[index]);

   //library.new_face(
   //   ft_instance,
   //   fonts[index],
   //   0,
   //   &ft_face);

   //freetype->SetSize(
   //   size);
   //freetype_font_engine->SetSize(
   //   size);

   //if (init)
   //{
   //   init = false;
   //
   //   for (auto c : default_char_set_)
   //   {
   //      freetype_font_engine->GetGlyphMetric(c);
   //   }
   //}

   //write_data();

   //library.set_pixel_sizes(
   //   ft_face,
   //   0,
   //   size);

   //glyph_max_top = 0;
   //glyph_max_height = 0;

   //uint32_t current_row { };
   //uint32_t current_height { };
   //uint32_t next_line { };

   //texture.reset(
   //   new uint8_t[tex_width * tex_height] { });

   //glyphs.reset(
   //   new glyph[128] { });

   //for (uint32_t i { 32 }; 127u > i; ++i)
   //{
   //   //library.load_char(
   //   //   ft_face,
   //   //   i,
   //   //   1 << 2);
   //   const auto glyph =
   //      freetype->GetGlyph(
   //         i);
   //
   //   glyphs[i].height = glyph->height;
   //   glyphs[i].width = glyph->width;
   //   const auto buffer = glyph->bitmap.data();
   //   glyphs[i].left = glyph->left;
   //   glyphs[i].top = glyph->top;
   //   glyphs[i].advance = glyph->advance;
   //
   //   //glyphs[i].height =
   //   //   GetBitmapData< 0, uint32_t >(
   //   //      ft_face);
   //   //glyphs[i].width =
   //   //   GetBitmapData< 4, uint32_t >(
   //   //      ft_face);
   //   //const auto buffer =
   //   //   GetBitmapData< 16, const uint8_t * >(
   //   //      ft_face);
   //   //glyphs[i].left =
   //   //   GetGlyphData< 144, int32_t >(
   //   //      ft_face);
   //   //glyphs[i].top =
   //   //   GetGlyphData< 148, int32_t >(
   //   //      ft_face);
   //   //glyphs[i].advance =
   //   //   GetGlyphData< 88, int32_t >(
   //   //      ft_face) / 64.0;
   //
   //   const auto width =
   //      glyphs[i].width;
   //   const auto height =
   //      glyphs[i].height;
   //
   //   if (current_row + width >= tex_width)
   //   {
   //      current_row = 0;
   //      current_height = next_line + 5;
   //   }
   //   
   //   if (current_height + height >= tex_height)
   //   {
   //      break;
   //   }
   //
   //   for (uint32_t i { }; height > i; ++i)
   //   {
   //      std::copy(
   //         buffer + width * (height - i - 1),
   //         buffer + width * (height - i),
   //         (texture.get() + (current_height + i) * tex_width + current_row));
   //   }
   //
   //   glyphs[i].uv_ll[0] =
   //      static_cast< double >(current_row) /
   //      static_cast< double >(tex_width - 1);
   //   glyphs[i].uv_ll[1] =
   //      static_cast< double >(current_height) /
   //      static_cast< double >(tex_width - 1);
   //
   //   glyphs[i].uv_ur[0] =
   //      static_cast< double >(current_row + width) /
   //      static_cast< double >(tex_width - 1);
   //   glyphs[i].uv_ur[1] =
   //      static_cast< double >(current_height + height) /
   //      static_cast< double >(tex_width - 1);
   //
   //   current_row += width + 5;
   //
   //   next_line =
   //      std::max(
   //         next_line,
   //         current_height + height);
   //
   //   glyph_max_height =
   //      std::max(
   //         glyph_max_height,
   //         height);
   //   glyph_max_top =
   //      std::max(
   //         glyph_max_top,
   //         glyph->top);
   //}

   //library.delete_face(
   //   ft_face);
   //
   //library.uninit(
   //   ft_instance);
}
//////////////////////////////////////////////////////////////////

class OpenGLWidget :
   public QOpenGLWidget
{
public:

   virtual void keyPressEvent(QKeyEvent *event) override
   {
      if ((event->key() == Qt::Key::Key_Plus ||
           event->key() == Qt::Key::Key_Minus) &&
           event->modifiers() & Qt::KeyboardModifier::ControlModifier)
      {
         if (event->key() == Qt::Key::Key_Plus)
            ++::size;
         else
            --::size;

         ::size =
            std::clamp(
               ::size,
               1u,
               1000u);

         font_test();

         update();
      }
      else if ((event->key() == Qt::Key::Key_BracketLeft ||
           event->key() == Qt::Key::Key_BracketRight) &&
           event->modifiers() & Qt::KeyboardModifier::ControlModifier)
      {
         if (event->key() == Qt::Key::Key_BracketLeft)
            ::scale -= 0.025f;
         else
            ::scale += 0.025f;

         ::scale =
            std::clamp(
               ::scale,
               0.1f,
               20.0f);

         update();
      }
      else if (!event->text().isEmpty() &&
               '0' <= event->text().at(0) &&
               event->text().at(0) <= '9' &&
               event->modifiers() & Qt::KeyboardModifier::ControlModifier)
      {
         font_index =
            event->text().toInt();

         font_test();

         update();
      }
      else if (event->key() == Qt::Key::Key_Return)
      {
         string += '\n';

         update();
      }
      else if (event->key() == Qt::Key::Key_Backspace)
      {
         if (!string.empty())
         {
            string.pop_back();

            update();
         }
      }
      else
      {
         string +=
            event->text().toStdString();

         update();
      }
   }

protected:
   // overrides from QOpenGLWidget
   virtual void initializeGL( ) override;
   virtual void resizeGL(
      const int32_t width,
      const int32_t height ) override;
   virtual void paintGL( ) override;

private:

};

void OpenGLWidget::initializeGL( )
{
   assert(glGetError() == GL_NO_ERROR);
}

void OpenGLWidget::resizeGL(
   const int32_t width,
   const int32_t height )
{
   assert(glGetError() == GL_NO_ERROR);
}

std::pair<
   std::pair< double, double >,
   std::pair< double, double > >
CalculateBoundingBox(
   const char * const text )
{
   std::pair<
      std::pair< double, double >,
      std::pair< double, double > >
      bounding_box { };

   const char * s { text };
   const char * const e { text + std::strlen(text) };

   double pen_x { };
   double pen_y { };

   for (; s != e; ++s)
   {
      if (*s == '\n')
      {
         pen_x = 0;
         pen_y -= freetype_font_engine->GetVerticalAdvance();
      }
      else
      {
         const auto metric =
         freetype_font_engine->GetGlyphMetric(
            *s);

         if (metric)
         {
            double old_pen_x = pen_x;
            pen_x += metric->advance;

            bounding_box.first.second =
               std::max(
                  pen_y - (double(metric->height) - metric->top) + metric->height,
                  bounding_box.first.second);

            bounding_box.second.first =
               std::max(
                  pen_x,
                  bounding_box.second.first);
            bounding_box.second.first =
               std::max(
                  old_pen_x + metric->left + metric->width,
                  bounding_box.second.first);
            bounding_box.second.second =
               std::min(
                  pen_y - (double(metric->height) - metric->top),
                  bounding_box.second.second);
         }
      }
   }

   return
      bounding_box;
}

uint32_t SetupTexture( )
{
   static uint32_t tid { 0 };

   if (font_engine_texture_map.texture_map.expired())
   {
      font_engine_texture_map =
         freetype_font_engine->GetGlyphTextureMap();

      auto tex_map =
         font_engine_texture_map.texture_map.lock();

      if (!tex_map) return 0;

      if (tid)
         glDeleteTextures(
            1,
            &tid);

      glGenTextures(
         1,
         &tid);
      glBindTexture(
         GL_TEXTURE_2D,
         tid);
      glPixelStorei(
         GL_UNPACK_ALIGNMENT,
         1);
      glTexImage2D(
         GL_TEXTURE_2D,
         0,
         GL_LUMINANCE,
         font_engine_texture_map.width,
         font_engine_texture_map.height,
         0,
         GL_RED,
         GL_UNSIGNED_BYTE,
         tex_map.get());
      glPixelStorei(
         GL_UNPACK_ALIGNMENT,
         4);
      reinterpret_cast< void (CALL_CONV *) ( GLenum ) >(
         wglGetProcAddress("glGenerateMipmap"))(
            GL_TEXTURE_2D);
      glTexParameteri(
         GL_TEXTURE_2D,
         GL_TEXTURE_WRAP_S,
         GL_CLAMP_TO_BORDER);
      glTexParameteri(
         GL_TEXTURE_2D,
         GL_TEXTURE_WRAP_T,
         GL_CLAMP_TO_BORDER);
      glTexParameteri(
         GL_TEXTURE_2D,
         GL_TEXTURE_MIN_FILTER,
         GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(
         GL_TEXTURE_2D,
         GL_TEXTURE_MAG_FILTER,
         GL_LINEAR);
      glBindTexture(
         GL_TEXTURE_2D,
         0);
      //texture.reset();

      assert(glGetError() == GL_NO_ERROR);
   }

   return tid;
}

void RenderText(
   const char * const text,
   const float x,
   const float y,
   const float scale)
{
   if (!freetype_font_engine) return;

   if (!text || *text == '\0') return;

   const size_t text_length =
      std::strlen(text);

   const char * s { text };
   const char * const e { text + text_length };

   std::vector< float > vertices;
   vertices.reserve(text_length * 3 * 6);
   std::vector< float > tex_coords;
   tex_coords.reserve(text_length * 2 * 6);

   //glBegin(
   //   GL_TRIANGLES);

   float pen_x { };
   float pen_y { };

   for (; s != e; ++s)
   {
      const int8_t c { *s };

      if (c == '\n')
      {
         pen_x = 0;
         pen_y -= freetype_font_engine->GetVerticalAdvance();
      }
      else
      {
         const auto g =
         //glyphs[c];
         freetype_font_engine->GetGlyphMetric(
            c);

         if (g)
         {
            if (c != ' ')
            {
               const float x =
                  pen_x + g->left;
               const float y =
                  pen_y - (static_cast< float >(g->height) - g->top);
               
               //glTexCoord2f(
               //   g->tex_coords.normalized.left,
               //   g->tex_coords.normalized.top);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.left);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.top);
               //glVertex3f(x, y + g->height, 0.0f);
               vertices.emplace_back(x);
               vertices.emplace_back(y + g->height);
               vertices.emplace_back(0.0f);
               //glTexCoord2f(
               //   g->tex_coords.normalized.left,
               //   g->tex_coords.normalized.bottom);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.left);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.bottom);
               //glVertex3f(x, y, 0.0f);
               vertices.emplace_back(x);
               vertices.emplace_back(y);
               vertices.emplace_back(0.0f);
               //glTexCoord2f(
               //   g->tex_coords.normalized.right,
               //   g->tex_coords.normalized.bottom);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.right);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.bottom);
               //glVertex3f(x + g->width, y, 0.0f);
               vertices.emplace_back(x + g->width);
               vertices.emplace_back(y);
               vertices.emplace_back(0.0f);

               //glTexCoord2f(
               //   g->tex_coords.normalized.left,
               //   g->tex_coords.normalized.top);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.left);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.top);
               //glVertex3f(x, y + g->height, 0.0f);
               vertices.emplace_back(x);
               vertices.emplace_back(y + g->height);
               vertices.emplace_back(0.0f);
               //glTexCoord2f(
               //   g->tex_coords.normalized.right,
               //   g->tex_coords.normalized.bottom);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.right);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.bottom);
               //glVertex3f(x + g->width, y, 0.0f);
               vertices.emplace_back(x + g->width);
               vertices.emplace_back(y);
               vertices.emplace_back(0.0f);
               //glTexCoord2f(
               //   g->tex_coords.normalized.right,
               //   g->tex_coords.normalized.top);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.right);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.top);
               //glVertex3f(x + g->width, y + g->height, 0.0f);
               vertices.emplace_back(x + g->width);
               vertices.emplace_back(y + g->height);
               vertices.emplace_back(0.0f);
            }

            pen_x += g->advance;
         }
      }
   }

   const uint32_t tid =
      SetupTexture();

   glEnable(
      GL_TEXTURE_2D);
   glBindTexture(
      GL_TEXTURE_2D,
      tid);

   glPushMatrix();

   glTranslatef(x, y, 0);
   glScalef(scale, scale, 1.0);

   glEnableClientState(
      GL_VERTEX_ARRAY);
   glEnableClientState(
      GL_TEXTURE_COORD_ARRAY);

   glVertexPointer(
      3, GL_FLOAT, 0, vertices.data());
   glTexCoordPointer(
      2, GL_FLOAT, 0, tex_coords.data());

   glDrawArrays(
      GL_TRIANGLES,
      0,
      vertices.size() / 3);

   glDisableClientState(
      GL_VERTEX_ARRAY);
   glDisableClientState(
      GL_TEXTURE_COORD_ARRAY);

   //glEnd();

   glBindTexture(
      GL_TEXTURE_2D,
      0);
   glDisable(
      GL_TEXTURE_2D);

   const auto bounding_box =
      CalculateBoundingBox(
         text);

   glBegin(
      GL_LINE_LOOP);

   glVertex3d(
      bounding_box.first.first,
      bounding_box.first.second,
      0.0);
   glVertex3d(
      bounding_box.second.first,
      bounding_box.first.second,
      0.0);
   glVertex3d(
      bounding_box.second.first,
      bounding_box.second.second,
      0.0);
   glVertex3d(
      bounding_box.first.first,
      bounding_box.second.second,
      0.0);

   glEnd();

   //glEnable(
   //   GL_TEXTURE_2D);

   glPopMatrix();
}

void OpenGLWidget::paintGL( )
{
   //glEnable(
   //   GL_BLEND);
   //glBlendFunc(
   //   GL_SRC_ALPHA,
   //   GL_ONE_MINUS_SRC_ALPHA);

   glClear(
      GL_COLOR_BUFFER_BIT |
      GL_DEPTH_BUFFER_BIT);

   int32_t w = width();
   int32_t h = height();

   glMatrixMode(
      GL_PROJECTION);
   glLoadIdentity();
   glOrtho(
      0.0, w,
      0.0, h,
      -1.0, 0.0);

   glMatrixMode(
      GL_MODELVIEW);
   glLoadIdentity();

   //glEnable(
   //   GL_TEXTURE_2D);
   //glBindTexture(
   //   GL_TEXTURE_2D,
   //   tid);

#define RENDER_AS_WORD_PROCESSOR 1
#if RENDER_AS_WORD_PROCESSOR

#define RENDER_FONT_TEXTURE 0

   const std::string s {
      "scale: " + std::to_string(scale) + "\n"
      "size: " + std::to_string(::size) + "\n"
      + string
   };

   RenderText(
      s.c_str(),
      0.0f, height() - freetype_font_engine->GetGlyphMaxTop() * scale,
      scale);

#if RENDER_FONT_TEXTURE
   
   glMatrixMode(
      GL_PROJECTION);
   glLoadIdentity();
   glMatrixMode(
      GL_MODELVIEW);
   glLoadIdentity();

   const uint32_t tid =
      SetupTexture();

   glEnable(
      GL_TEXTURE_2D);
   glBindTexture(
      GL_TEXTURE_2D,
      tid);

   glBegin(
      GL_TRIANGLES);

   glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, 0.0f);
   glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
   glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.0f);

   glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, 0.0f);
   glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.0f);
   glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, 0.0f);

   glEnd();

   glBindTexture(
      GL_TEXTURE_2D,
      0);
   glDisable(
      GL_TEXTURE_2D);

#endif

#else
   static float velocity[] { 105.0f, 40.0f };
   static float position[] { width() / 2.0f, height() / 2.0f };
   static auto time = std::chrono::steady_clock::now();

   glPushMatrix();

   glTranslatef(position[0], position[1], 0.0f);

   glColor3f(1, 0, 0);
   
   glBegin(GL_TRIANGLES);
   glVertex3f(-6, 6, 0);
   glVertex3f(-6, -6, 0);
   glVertex3f(6, -6, 0);
   glVertex3f(-6, 6, 0);
   glVertex3f(6, -6, 0);
   glVertex3f(6, 6, 0);
   glEnd();

   glColor3f(1, 1, 1);

   auto text =
      std::to_string(position[0]) +
      "\n" +
      std::to_string(position[1]);

   double size[2] { };
   double size_line[2] { };

   for (const auto c : text)
   {
      const auto g =
         freetype_font_engine->GetGlyphMetric(
            c);

      if (c == '\n')
      {
         size_line[0] = 0.0;
         size_line[1] = size[1];
         continue;
      }

      size_line[0] += g->advance;
      size[0] = std::max(size[0], size_line[0]);
      size[1] = std::max(size[1], size_line[1] + g->height);
   }

   RenderText(
      text.c_str(),
      size[0] / -2.0 * scale,
      freetype_font_engine->GetVerticalAdvance() * scale + 12,
      scale);

   glPopMatrix();

   auto now = std::chrono::steady_clock::now();
   auto delta = now - time;

   time = now;

   position[0] +=
      velocity[0] *
      std::chrono::duration_cast< std::chrono::duration< float > >(delta).count();
   position[1] +=
      velocity[1] *
      std::chrono::duration_cast< std::chrono::duration< float > >(delta).count();

   if (0.0f >= position[0])
   {
      position[0] = 0.0f;
      velocity[0] *= -1.0f;
   }
   else if (position[0] >= width() - 1)
   {
      position[0] = width() - 1;
      velocity[0] *= -1.0f;
   }

   if (0.0f >= position[1])
   {
      position[1] = 0.0f;
      velocity[1] *= -1.0f;
   }
   else if (position[1] >= height() - 1)
   {
      position[1] = height() - 1;
      velocity[1] *= -1.0f;
   }

   update();

#endif // RENDER_AS_WORD_PROCESSOR

   //glBegin(
   //   GL_TRIANGLES);
   //
   //glTexCoord2f(0.0f, 1.0f);
   //glVertex3f(-1.0f, 1.0f, 0.0);
   //glTexCoord2f(0.0f, 0.0f);
   //glVertex3f(-1.0f, -1.0f, 0.0);
   //glTexCoord2f(1.0f, 0.0f);
   //glVertex3f(1.0f, -1.0f, 0.0);
   //
   //glTexCoord2f(0.0f, 1.0f);
   //glVertex3f(-1.0f, 1.0f, 0.0);
   //glTexCoord2f(1.0f, 0.0f);
   //glVertex3f(1.0f, -1.0f, 0.0);
   //glTexCoord2f(1.0f, 1.0f);
   //glVertex3f(1.0f, 1.0f, 0.0);
   //
   //glEnd();

   //glBindTexture(
   //   GL_TEXTURE_2D,
   //   0);

   //glDisable(
   //   GL_BLEND);
}

int32_t main(
   const int32_t argc,
   const char * const (&argv)[] )
{
   //auto surface_format =
   //   QSurfaceFormat::defaultFormat();
   //surface_format.setRenderableType(
   //   QSurfaceFormat::RenderableType::OpenGL);
   //surface_format.setProfile(
   //   QSurfaceFormat::OpenGLContextProfile::CompatibilityProfile);
   //surface_format.setVersion(
   //   4, 5);
   //
   //QSurfaceFormat::setDefaultFormat(
   //   surface_format);

   int32_t argc_ { argc };
   char ** argv_ { const_cast< char ** >(argv) };

   QApplication application {
      argc_,
      argv_
   };

   font_test();

   // add surface format capabilities here

   OpenGLWidget ogl_widget;

   ogl_widget.show();

   return
      QApplication::exec();
}
