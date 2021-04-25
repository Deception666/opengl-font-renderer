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

#include "font_engine_factory.h"
#include "font_engine_freetype.h"
#include "font_engine_default.h"
#include "font_texture_manager.h"

float scale { 1.0f };
uint32_t size { 48 };
size_t font_index { 0 };

static std::string string;

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

   const char * const fonts[] {
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
         if (c != '\n')
            freetype_font_engine->GetGlyphMetric(
               c);
      }
   }
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
   static std::shared_ptr< uint32_t > texture;

   if (font_engine_texture_map.texture_map.expired())
   {
      font_engine_texture_map =
         freetype_font_engine->GetGlyphTextureMap();

      auto tex_map =
         font_engine_texture_map.texture_map.lock();

      if (!tex_map) return 0;

      texture =
         opengl::font_texture_manager::CreateTexture(
            font_engine_texture_map.width,
            font_engine_texture_map.height,
            freetype_font_engine->GetFont(),
            freetype_font_engine->GetSize());

      opengl::font_texture_manager::UpdateTexture(
         texture,
         font_engine_texture_map.width,
         font_engine_texture_map.height,
         tex_map);

      tid = *texture;
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
               
               tex_coords.emplace_back(
                  g->tex_coords.normalized.left);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.top);
               vertices.emplace_back(x);
               vertices.emplace_back(y + g->height);
               vertices.emplace_back(0.0f);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.left);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.bottom);
               vertices.emplace_back(x);
               vertices.emplace_back(y);
               vertices.emplace_back(0.0f);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.right);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.bottom);
               vertices.emplace_back(x + g->width);
               vertices.emplace_back(y);
               vertices.emplace_back(0.0f);

               tex_coords.emplace_back(
                  g->tex_coords.normalized.left);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.top);
               vertices.emplace_back(x);
               vertices.emplace_back(y + g->height);
               vertices.emplace_back(0.0f);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.right);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.bottom);
               vertices.emplace_back(x + g->width);
               vertices.emplace_back(y);
               vertices.emplace_back(0.0f);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.right);
               tex_coords.emplace_back(
                  g->tex_coords.normalized.top);
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

#define RENDER_AS_WORD_PROCESSOR 1
#if RENDER_AS_WORD_PROCESSOR

#define RENDER_FONT_TEXTURE 0
#define RENDER_CURRENT_TIME 1

#if !RENDER_CURRENT_TIME

   const std::string s {
      "scale: " + std::to_string(scale) + "\n"
      "size: " + std::to_string(::size) + "\n"
      + string
   };

   RenderText(
      s.c_str(),
      0.0f, height() - freetype_font_engine->GetGlyphMaxTop() * scale,
      scale);

#else

   using days = std::chrono::duration< int, std::ratio< 86400 > >;

   auto time_now =
      std::chrono::system_clock::now().time_since_epoch();

   auto yday = std::chrono::duration_cast< days >(
      std::chrono::system_clock::now().time_since_epoch());
   auto hr1 = std::chrono::duration_cast< std::chrono::hours >(time_now);
   auto hr2 = std::chrono::duration_cast< std::chrono::hours >(yday);
   auto hr = hr1 - hr2;

   auto hour = std::chrono::duration_cast< std::chrono::hours >(hr);
   auto minute = std::chrono::duration_cast< std::chrono::minutes >(time_now);
   auto second = std::chrono::duration_cast< std::chrono::seconds >(time_now);
   auto msec = std::chrono::duration_cast< std::chrono::milliseconds >(time_now);

   // completely wrong do to needing to validate months but who cares for this...
   TIME_ZONE_INFORMATION info { };
   GetTimeZoneInformation(
      &info);

   auto hour_24 =
      (hour.count() + 24 - (info.Bias + info.DaylightBias) / 60) % 24;

   std::string am_pm {
      hour_24 >= 12 ?
      "PM" : "AM" };

   auto milisec =
      static_cast< uint64_t >(msec.count() % 1000 / 10);

   std::stringstream ss;
   ss
      << std::setfill('0')
      << std::setw(2)
      << (hour_24 % 12 == 0 ? 12 : hour_24 % 12)
      << ":"
      << std::setw(2)
      << minute.count() % 60
      << ":"
      << std::setw(2)
      << second.count() % 60
      << ":"
      << std::setw(2)
      << milisec
      << " "
      << am_pm;
   
   RenderText(
      ss.str().c_str(),
      width() / 2.0f - 150,
      height() / 2.0f,
      scale);

   update();

#endif

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
