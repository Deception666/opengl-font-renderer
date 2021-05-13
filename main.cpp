#include <QtWidgets/QApplication>
#include <QtWidgets/QOpenGLWidget>

#include <cstdint>

//////////////////////////////////////////////////////////////////
#include <memory>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <QtGui/QKeyEvent>
#include <QtGui/QImageWriter>

// #include <ft2build.h>
// #include <freetype/freetype.h>

#include "text.h"
#include "error_reporting.h"

float scale { 1.0f };
uint32_t size { 48 };
size_t font_index { 0 };

static std::string string;

std::unique_ptr< opengl::Text > text;

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
      "bkant.ttf",
      "wingding.ttf"
   };

   size_t index =
      std::clamp(
         font_index,
         static_cast< size_t >(0),
         std::size(fonts) - 1);
   
   if (!text)
   {
      opengl::SetErrorCallback(
         [ ] (
            const char * const error )
         {
            std::cout << error << "\n\n";
         });

      text =
         std::make_unique< opengl::Text >(
            fonts[index],
            size);

      text->SetColor(1.0f, 1.0f, 1.0f);
   }
   else
   {
      text->SetFont(
         fonts[index],
         size);
   }
}
//////////////////////////////////////////////////////////////////

class OpenGLWidget :
   public QOpenGLWidget
{
public:
   virtual ~OpenGLWidget( )
   {
      makeCurrent();

      text.reset();

      doneCurrent();
   }

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
   std::ignore = width;
   std::ignore = height;

   assert(glGetError() == GL_NO_ERROR);
}

//std::pair<
//   std::pair< double, double >,
//   std::pair< double, double > >
//CalculateBoundingBox(
//   const char * const text )
//{
//   std::pair<
//      std::pair< double, double >,
//      std::pair< double, double > >
//      bounding_box { };
//
//   const char * s { text };
//   const char * const e { text + std::strlen(text) };
//
//   double pen_x { };
//   double pen_y { };
//
//   for (; s != e; ++s)
//   {
//      if (*s == '\n')
//      {
//         pen_x = 0;
//         pen_y -= freetype_font_engine->GetVerticalAdvance();
//      }
//      else
//      {
//         const auto metric =
//         freetype_font_engine->GetGlyphMetric(
//            *s);
//
//         if (metric)
//         {
//            double old_pen_x = pen_x;
//            pen_x += metric->advance;
//
//            bounding_box.first.second =
//               std::max(
//                  pen_y - (double(metric->height) - metric->top) + metric->height,
//                  bounding_box.first.second);
//
//            bounding_box.second.first =
//               std::max(
//                  pen_x,
//                  bounding_box.second.first);
//            bounding_box.second.first =
//               std::max(
//                  old_pen_x + metric->left + metric->width,
//                  bounding_box.second.first);
//            bounding_box.second.second =
//               std::min(
//                  pen_y - (double(metric->height) - metric->top),
//                  bounding_box.second.second);
//         }
//      }
//   }
//
//   return
//      bounding_box;
//}

void OpenGLWidget::paintGL( )
{
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
#define RENDER_CURRENT_TIME 0

#if !RENDER_CURRENT_TIME

   std::string s {
      "scale: " + std::to_string(scale) + "\n"
      "size: " + std::to_string(::size) + "\n"
      + string
   };

   if (text)
   {
      text->SetText(std::move(s));
      text->SetScale(scale);
      text->SetPosition(0.0f, height() - text->GetFontMaxTop() * scale, 0.0f);
      text->Render();
   }

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
 
   text->SetScale(scale);
   text->SetText(ss.str());
   text->SetPosition(
      width() / 2.0f - 150.0f,
      height() / 2.0f,
      0.0f);
   text->Render();

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
      2u; // get from texture in text

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

   auto stext =
      std::to_string(position[0]) +
      "\n" +
      std::to_string(position[1]);

//   RenderText(
//      text.c_str(),
//      size[0] / -2.0 * scale,
//      freetype_font_engine->GetVerticalAdvance() * scale + 12,
//      scale);

   glPopMatrix();

   text->SetText(std::move(stext));
   text->SetPosition(
      position[0],
      position[1] + 12,
      0.0f);
   text->SetScale(scale);
   text->Render();

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
