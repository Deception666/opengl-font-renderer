//#include <QtWidgets/QApplication>
//#include <QtWidgets/QOpenGLWidget>
//
//#include <cstdint>
//
////////////////////////////////////////////////////////////////////
//#include <memory>
//#include <cstring>
//#include <algorithm>
//#include <fstream>
//#include <iomanip>
//#include <iostream>
//
//#include <QtGui/QKeyEvent>
//#include <QtGui/QImageWriter>
//
////#include <freetype2/ft2build.h>
////#include <freetype/freetype.h>
//
//#include "opengl-text/text.h"
//#include "opengl-text/error_reporting.h"
//
////#include "src/font_engine_freetype.h"
////#include "src/font_engine_type.h"
//
//float scale { 1.0f };
//uint32_t size { 48 };
//size_t font_index { 0 };
//
//static std::string string;
//
//std::unique_ptr< opengl::Text > text;
//std::unique_ptr< opengl::Text > text2;
//std::unique_ptr< opengl::Text > text3;
//
////void write_data( )
////{
////   opengl::FontEngineFreeType freetype_font_engine;
////   freetype_font_engine.Initialize();
////   freetype_font_engine.SetFont("DejaVuSans.ttf");
////   freetype_font_engine.SetSize(104);
////
////   std::ofstream o2;
////
////   o2.open(
////      "metrics.txt",
////      std::ios_base::out |
////      std::ios_base::trunc);
////   
////   char default_char_set_[127 - 32] { };
////   std::iota(
////      std::begin(default_char_set_),
////      std::end(default_char_set_),
////      32);
////
////   for (const auto c : default_char_set_)
////   {
////      const auto metric =
////         freetype_font_engine.GetGlyphMetric(c);
////
////      o2
////         << "{ "
////         << metric->width << ", "
////         << metric->height << ", "
////         << metric->top << ", "
////         << metric->left << ", "
////         << metric->advance << ", "
////         << "{ { "
////         << metric->tex_coords.absolute.top << ", "
////         << metric->tex_coords.absolute.bottom << ", "
////         << metric->tex_coords.absolute.left << ", "
////         << metric->tex_coords.absolute.right
////         << " }, "
////         << "{ "
////         << std::setprecision(10)
////         << metric->tex_coords.normalized.top << "f, "
////         << metric->tex_coords.normalized.bottom << "f, "
////         << metric->tex_coords.normalized.left << "f, "
////         << metric->tex_coords.normalized.right << "f"
////         << " } } },\n";
////   }
////
////   o2
////      << "VA: " << freetype_font_engine.GetVerticalAdvance() << "\n"
////      << "MH: " << freetype_font_engine.GetGlyphMaxHeight() << "\n"
////      << "MW: " << freetype_font_engine.GetGlyphMaxWidth() << "\n"
////      << "MT: " << freetype_font_engine.GetGlyphMaxTop() << "\n"
////      << "SZ: " << freetype_font_engine.GetSize() << "\n\n";
////
////   std::ofstream o;
////
////   o.open(
////      "texture.txt",
////      std::ios_base::out |
////      std::ios_base::trunc);
////
////   o
////      << std::hex
////      << std::uppercase
////      << std::internal/*
////      << std::setw(2)
////      << std::setfill('0')*/;
////
////   const auto texture_map =
////      freetype_font_engine.GetGlyphTextureMap();
////   
////   const auto map =
////      texture_map.texture_map.lock();
////
////   for (uint32_t h { }; texture_map.height > h; ++h)
////   {
////      for (uint32_t r { }; texture_map.width > r; ++r)
////      {
////         o
////            << "0x"
////            << std::setw(2)
////            << std::setfill('0')
////            << static_cast< uint32_t >(
////               *(map.get() + (texture_map.width * h + r)))
////            << ", ";
////      }
////
////      o << "\n";
////   }
////
////   QImage image {
////      static_cast< int32_t >(texture_map.width),
////      static_cast< int32_t >(texture_map.height),
////      QImage::Format::Format_Grayscale8
////   };
////
////   std::copy(
////      map.get(),
////      map.get() + texture_map.width * texture_map.height,
////      image.bits());
////
////   QImageWriter image_writer {
////      "image.png"
////   };
////
////   image_writer.write(
////      image);
////}
//
//void font_test( )
//{
//
//   //write_data();
//
//#if _WIN32
//   const char * const fonts[] {
//      "arial.ttf",
//      "itcedscr.ttf",
//      "bradhitc.ttf",
//      "consola.ttf",
//      "times.ttf",
//      "calibri.ttf",
//      "cambria.ttc",
//      "bkant.ttf",
//      "wingding.ttf"
//   };
//#elif __linux__
//   const char * const fonts[] {
//      "DejaVuSans.ttf",
//      "ani.ttf",
//      "D050000L.otf",
//      "Karumbi-Regular.ttf",
//      "NimbusMonoPS-Regular.t1",
//      "DroidSansFallbackFull.ttf",
//      "Purisa.ttf",
//      "NotoColorEmoji.ttf",
//      "Chilanka-Regular.otf",
//      "c0419bt_.pfb"
//   };
//#endif
//
//   size_t index =
//      std::clamp(
//         font_index,
//         static_cast< size_t >(0),
//         std::size(fonts) - 1);
//   
//   if (!text)
//   {
//      opengl::SetErrorCallback(
//         [ ] (
//            const char * const error )
//         {
//            std::cout << error << "\n\n";
//         });
//
//      text =
//         std::make_unique< opengl::Text >(
//            fonts[index],
//            size);
//      text2 =
//         std::make_unique< opengl::Text >(
//            fonts[index],
//            size);
//      text3 =
//         std::make_unique< opengl::Text >(
//            fonts[index],
//            size);
//
//      text->SetColor(1.0f, 1.0f, 1.0f);
//      text2->SetColor(1.0f, 0.0f, 0.0f);
//      text3->SetColor(0.0f, 1.0f, 0.0f);
//   }
//   else
//   {
//      text->SetFont(
//         fonts[index],
//         size);
//      text2->SetFont(
//         fonts[index],
//         size);
//      text3->SetFont(
//         fonts[index],
//         size);
//   }
//}
////////////////////////////////////////////////////////////////////
//
//class OpenGLWidget :
//   public QOpenGLWidget
//{
//public:
//   virtual ~OpenGLWidget( )
//   {
//      makeCurrent();
//
//      text.reset();
//      text2.reset();
//      text3.reset();
//
//      doneCurrent();
//   }
//
//   virtual void keyPressEvent(QKeyEvent *event) override
//   {
//      if ((event->key() == Qt::Key::Key_Plus ||
//           event->key() == Qt::Key::Key_Minus) &&
//           event->modifiers() & Qt::KeyboardModifier::ControlModifier)
//      {
//         if (event->key() == Qt::Key::Key_Plus)
//            ++::size;
//         else
//            --::size;
//
//         ::size =
//            std::clamp(
//               ::size,
//               1u,
//               1000u);
//
//         font_test();
//
//         update();
//      }
//      else if ((event->key() == Qt::Key::Key_BracketLeft ||
//           event->key() == Qt::Key::Key_BracketRight) &&
//           event->modifiers() & Qt::KeyboardModifier::ControlModifier)
//      {
//         if (event->key() == Qt::Key::Key_BracketLeft)
//            ::scale -= 0.025f;
//         else
//            ::scale += 0.025f;
//
//         ::scale =
//            std::clamp(
//               ::scale,
//               0.1f,
//               20.0f);
//
//         update();
//      }
//      else if (!event->text().isEmpty() &&
//               '0' <= event->text().at(0) &&
//               event->text().at(0) <= '9' &&
//               event->modifiers() & Qt::KeyboardModifier::AltModifier)
//      {
//         font_index =
//            event->text().toInt();
//
//         font_test();
//
//         update();
//      }
//      else if (event->key() == Qt::Key::Key_Return)
//      {
//         string += '\n';
//
//         update();
//      }
//      else if (event->key() == Qt::Key::Key_Backspace)
//      {
//         if (!string.empty())
//         {
//            string.pop_back();
//
//            update();
//         }
//      }
//      else
//      {
//         string +=
//            event->text().toStdString();
//
//         update();
//      }
//   }
//
//protected:
//   // overrides from QOpenGLWidget
//   virtual void initializeGL( ) override;
//   virtual void resizeGL(
//      const int32_t width,
//      const int32_t height ) override;
//   virtual void paintGL( ) override;
//
//private:
//
//};
//
//void OpenGLWidget::initializeGL( )
//{
//   assert(glGetError() == GL_NO_ERROR);
//}
//
//void OpenGLWidget::resizeGL(
//   const int32_t width,
//   const int32_t height )
//{
//   std::ignore = width;
//   std::ignore = height;
//
//   assert(glGetError() == GL_NO_ERROR);
//}
//
////std::pair<
////   std::pair< double, double >,
////   std::pair< double, double > >
////CalculateBoundingBox(
////   const char * const text )
////{
////   std::pair<
////      std::pair< double, double >,
////      std::pair< double, double > >
////      bounding_box { };
////
////   const char * s { text };
////   const char * const e { text + std::strlen(text) };
////
////   double pen_x { };
////   double pen_y { };
////
////   for (; s != e; ++s)
////   {
////      if (*s == '\n')
////      {
////         pen_x = 0;
////         pen_y -= freetype_font_engine->GetVerticalAdvance();
////      }
////      else
////      {
////         const auto metric =
////         freetype_font_engine->GetGlyphMetric(
////            *s);
////
////         if (metric)
////         {
////            double old_pen_x = pen_x;
////            pen_x += metric->advance;
////
////            bounding_box.first.second =
////               std::max(
////                  pen_y - (double(metric->height) - metric->top) + metric->height,
////                  bounding_box.first.second);
////
////            bounding_box.second.first =
////               std::max(
////                  pen_x,
////                  bounding_box.second.first);
////            bounding_box.second.first =
////               std::max(
////                  old_pen_x + metric->left + metric->width,
////                  bounding_box.second.first);
////            bounding_box.second.second =
////               std::min(
////                  pen_y - (double(metric->height) - metric->top),
////                  bounding_box.second.second);
////         }
////      }
////   }
////
////   return
////      bounding_box;
////}
//
//void OpenGLWidget::paintGL( )
//{
//   glClear(
//      GL_COLOR_BUFFER_BIT |
//      GL_DEPTH_BUFFER_BIT);
//
//   int32_t w = width();
//   int32_t h = height();
//
//   glMatrixMode(
//      GL_PROJECTION);
//   glLoadIdentity();
//   glOrtho(
//      0.0, w,
//      0.0, h,
//      -1.0, 0.0);
//
//   glMatrixMode(
//      GL_MODELVIEW);
//   glLoadIdentity();
//
//#define RENDER_AS_WORD_PROCESSOR 1
//#if RENDER_AS_WORD_PROCESSOR
//
//#define RENDER_FONT_TEXTURE 0
//#define RENDER_CURRENT_TIME 0
//
//#if !RENDER_CURRENT_TIME
//
//   std::string s {
//      "scale: " + std::to_string(scale) + "\n"
//      "size: " + std::to_string(::size) + "\n"
//      "name: " + text->GetFont() + "\n"
//      + std::to_string(text->GetBoundingBox().GetWidth()) + "\n"
//      + std::to_string(text->GetBoundingBox().GetHeight()) + "\n"
//      + string
//   };
//
//   if (text)
//   {
//      text->SetText(std::move(s));
//      text->SetScale(scale);
//      text->SetPosition(
//         /*std::abs(std::get< 0 >(text->GetBoundingBox().GetUpperLeftFront())) -
//         text->GetPosition()[0]*/
//         0.0f,
//         height() /*- text->GetFontMaxTop() * scale*/,
//         0.0f);
//      //text->SetAlignment(
//      //   opengl::Text::Align::VCENTER |
//      //   opengl::Text::Align::HCENTER);
//      text->Render();
//   }
//
//   glPointSize(6);
//   glBegin(GL_POINTS);
//   glVertex3fv(text->GetPosition());
//   glEnd();
//
//#else
//
//   using days = std::chrono::duration< int, std::ratio< 86400 > >;
//
//   auto time_now =
//      std::chrono::system_clock::now().time_since_epoch();
//
//   auto yday = std::chrono::duration_cast< days >(
//      std::chrono::system_clock::now().time_since_epoch());
//   auto hr1 = std::chrono::duration_cast< std::chrono::hours >(time_now);
//   auto hr2 = std::chrono::duration_cast< std::chrono::hours >(yday);
//   auto hr = hr1 - hr2;
//
//   auto hour = std::chrono::duration_cast< std::chrono::hours >(hr);
//   auto minute = std::chrono::duration_cast< std::chrono::minutes >(time_now);
//   auto second = std::chrono::duration_cast< std::chrono::seconds >(time_now);
//   auto msec = std::chrono::duration_cast< std::chrono::milliseconds >(time_now);
//
//   // completely wrong do to needing to validate months but who cares for this...
//   TIME_ZONE_INFORMATION info { };
//   GetTimeZoneInformation(
//      &info);
//
//   auto hour_24 =
//      (hour.count() + 24 - (info.Bias + info.DaylightBias) / 60) % 24;
//
//   std::string am_pm {
//      hour_24 >= 12 ?
//      "PM" : "AM" };
//
//   auto milisec =
//      static_cast< uint64_t >(msec.count() % 1000 / 10);
//
//   std::stringstream ss;
//   ss
//      << std::setfill('0')
//      << std::setw(2)
//      << (hour_24 % 12 == 0 ? 12 : hour_24 % 12)
//      << ":"
//      << std::setw(2)
//      << minute.count() % 60
//      << ":"
//      << std::setw(2)
//      << second.count() % 60
//      << ":"
//      << std::setw(2)
//      << milisec
//      << " "
//      << am_pm;
// 
//   text->SetScale(scale);
//   text->SetText(ss.str());
//   text->SetPosition(
//      width() / 2.0f - 150.0f,
//      height() / 2.0f,
//      0.0f);
//   text->Render();
//
//   update();
//
//#endif
//
//#if RENDER_FONT_TEXTURE
//   
//   glMatrixMode(
//      GL_PROJECTION);
//   glLoadIdentity();
//   glMatrixMode(
//      GL_MODELVIEW);
//   glLoadIdentity();
//
//   const uint32_t tid =
//      2u; // get from texture in text
//
//   glEnable(
//      GL_TEXTURE_2D);
//   glBindTexture(
//      GL_TEXTURE_2D,
//      tid);
//
//   glBegin(
//      GL_TRIANGLES);
//
//   glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, 0.0f);
//   glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
//   glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.0f);
//
//   glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, 0.0f);
//   glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.0f);
//   glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, 0.0f);
//
//   glEnd();
//
//   glBindTexture(
//      GL_TEXTURE_2D,
//      0);
//   glDisable(
//      GL_TEXTURE_2D);
//
//#endif
//
//#else
//   static float velocity[] { 105.0f, 40.0f };
//   static float position[] { width() / 2.0f, height() / 2.0f };
//   static auto time = std::chrono::steady_clock::now();
//
//   glPushMatrix();
//
//   glTranslatef(position[0], position[1], 0.0f);
//
//   glColor3f(1, 0, 0);
//   
//   glBegin(GL_TRIANGLES);
//   glVertex3f(-6, 6, 0);
//   glVertex3f(-6, -6, 0);
//   glVertex3f(6, -6, 0);
//   glVertex3f(-6, 6, 0);
//   glVertex3f(6, -6, 0);
//   glVertex3f(6, 6, 0);
//   glEnd();
//
//   glColor3f(1, 1, 1);
//
//   auto stext =
//      std::to_string(position[0]);
//
////   RenderText(
////      text.c_str(),
////      size[0] / -2.0 * scale,
////      freetype_font_engine->GetVerticalAdvance() * scale + 12,
////      scale);
//
//   glPopMatrix();
//
//   text->SetText(std::move(stext));
//   text->SetPosition(
//      position[0] + 12,
//      position[1],
//      0.0f);
//   text->SetScale(scale);
//   text->SetAlignment(
//      opengl::Text::Align::LEFT |
//      opengl::Text::Align::VCENTER);
//   text->Render();
//
//   stext =
//      std::to_string(position[1]);
//
//   text2->SetText(std::move(stext));
//   text2->SetPosition(
//      position[0] - 12,
//      position[1],
//      0.0f);
//   text2->SetScale(scale);
//   text2->SetAlignment(
//      opengl::Text::Align::RIGHT |
//      opengl::Text::Align::VCENTER);
//   text2->Render();
//
//   stext =
//      std::to_string(velocity[1]);
//
//   text3->SetText(std::move(stext));
//   text3->SetPosition(
//      position[0],
//      position[1] + 15,
//      0.0f);
//   text3->SetScale(scale);
//   text3->SetAlignment(
//      opengl::Text::Align::BOTTOM |
//      opengl::Text::Align::HCENTER);
//   text3->Render();
//
//   auto now = std::chrono::steady_clock::now();
//   auto delta = now - time;
//
//   time = now;
//
//   position[0] +=
//      velocity[0] *
//      std::chrono::duration_cast< std::chrono::duration< float > >(delta).count();
//   position[1] +=
//      velocity[1] *
//      std::chrono::duration_cast< std::chrono::duration< float > >(delta).count();
//
//   if (0.0f >= position[0])
//   {
//      position[0] = 0.0f;
//      velocity[0] *= -1.0f;
//   }
//   else if (position[0] >= width() - 1)
//   {
//      position[0] = width() - 1;
//      velocity[0] *= -1.0f;
//   }
//
//   if (0.0f >= position[1])
//   {
//      position[1] = 0.0f;
//      velocity[1] *= -1.0f;
//   }
//   else if (position[1] >= height() - 1)
//   {
//      position[1] = height() - 1;
//      velocity[1] *= -1.0f;
//   }
//
//   update();
//
//#endif // RENDER_AS_WORD_PROCESSOR
//
//   const auto bb =
//      text->GetBoundingBox();
//
//   const auto delta_h =
//      std::get< 1 >(bb.GetUpperLeftFront()) -
//      text->GetPosition()[1];
//
//   glPushMatrix();
//
//   glBegin(GL_LINE_LOOP);
//
//   glVertex3f(
//      std::get< 0 >(bb.GetUpperLeftFront()),
//      std::get< 1 >(bb.GetUpperLeftFront()),
//      0.0f);
//   glVertex3f(
//      std::get< 0 >(bb.GetUpperLeftFront()),
//      std::get< 1 >(bb.GetLowerRightFront()),
//      0.0f);
//   glVertex3f(
//      std::get< 0 >(bb.GetLowerRightFront()),
//      std::get< 1 >(bb.GetLowerRightFront()),
//      0.0f);
//   glVertex3f(
//      std::get< 0 >(bb.GetLowerRightFront()),
//      std::get< 1 >(bb.GetUpperLeftFront()),
//      0.0f);
//
//   glEnd();
//
//   glPopMatrix();
//}

#include "scene.h"
#include "word_processor_scene.h"

#include <opengl-text/error_reporting.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QOpenGLWidget>

#include <QtGui/QKeyEvent>

#include <QtCore/QObject>

#include <algorithm>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>

class QKeyEvent;

#if _WIN32
static const char * const fonts[] {
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
#elif __linux__
static const char * const fonts[] {
   "DejaVuSans.ttf",
   "ani.ttf",
   "D050000L.otf",
   "Karumbi-Regular.ttf",
   "NimbusMonoPS-Regular.t1",
   "DroidSansFallbackFull.ttf",
   "Purisa.ttf",
   "NotoColorEmoji.ttf",
   "Chilanka-Regular.otf",
   "c0419bt_.pfb"
};
#endif

void OpenGLTextErrorCallback(
   const char * const error ) noexcept
{
   std::cout << error << "\n";
}

class SceneWidget :
   public QOpenGLWidget
{
public:
   SceneWidget( ) noexcept;
   virtual ~SceneWidget( ) noexcept;

protected:
   // overrides from QOpenGLWidget
   virtual void initializeGL( ) override;
   virtual void resizeGL(
      const int32_t width,
      const int32_t height ) override;
   virtual void paintGL( ) override;

   // event overrides from QWidget
   virtual void keyPressEvent(
      QKeyEvent * event ) override;

private:
   std::unique_ptr<
      Scene,
      std::function< void ( Scene * const ) > >
      scene_;

   uint32_t current_font_size_;
   const char * current_font_filename_;

};

SceneWidget::SceneWidget( ) noexcept :
current_font_size_ { 48 },
current_font_filename_ { fonts[0] }
{
}

SceneWidget::~SceneWidget( ) noexcept
{
}

void SceneWidget::initializeGL( )
{
   opengl::SetErrorCallback(
      &OpenGLTextErrorCallback);

   try
   {
      scene_ =
         decltype(scene_) {
            new WordProcessorScene,
            [ this ] (
               Scene * const scene ) noexcept
            {
               makeCurrent();
               
               delete scene;

               doneCurrent();
            }
         };

      if (!scene_->Initialize())
      {
         scene_ = nullptr;

         throw
            std::runtime_error {
               "Scene did not initialize properly!"
            };
      }

      QObject::connect(
         scene_.get(),
         &Scene::Redraw,
         this,
         static_cast< void (QWidget::*) ( ) >(
            &SceneWidget::update));

      scene_->SetFont(
         current_font_filename_,
         current_font_size_);
   }
   catch (const std::exception & e)
   {
      OpenGLTextErrorCallback(
         e.what());
   }
}

void SceneWidget::resizeGL(
   const int32_t width,
   const int32_t height )
{
   if (scene_)
   {
      scene_->Resize(
         width,
         height);
   }
}

void SceneWidget::paintGL( )
{
   if (scene_)
   {
      scene_->Render();
   }
}

void SceneWidget::keyPressEvent(
   QKeyEvent * event )
{
   if (scene_)
   {
      const auto ctrl_alt_modifiers =
         Qt::KeyboardModifier::AltModifier |
         Qt::KeyboardModifier::ControlModifier;

      if ((event->modifiers() & ctrl_alt_modifiers) == ctrl_alt_modifiers &&
          (event->key() == Qt::Key::Key_Equal ||
           event->key() == Qt::Key::Key_Minus))
      {
         if (event->key() == Qt::Key::Key_Equal)
         {
            ++current_font_size_;
         }
         else
         {
            current_font_size_ =
               std::max(
                  --current_font_size_,
                  1u);
         }

         scene_->SetFont(
            current_font_filename_,
            current_font_size_);

         update();
      }
      else if ((event->modifiers() & Qt::KeyboardModifier::AltModifier) ==
               Qt::KeyboardModifier::AltModifier &&
               Qt::Key::Key_0 <= event->key() && event->key() <= Qt::Key::Key_9)
      {
         const size_t font_index =
            event->key() - Qt::Key::Key_0;

         if (std::size(fonts) > font_index)
         {
            current_font_filename_ =
               fonts[font_index];

            scene_->SetFont(
               current_font_filename_,
               current_font_size_);

            update();
         }
      }
      else
      {
         scene_->KeyPressed(
            *event);
      }
   }
}

int32_t main(
   const int32_t argc,
   const char * const (&argv)[] )
{
   int32_t argc_ { argc };
   char ** argv_ { const_cast< char ** >(argv) };

   QApplication application {
      argc_,
      argv_
   };

   SceneWidget scene_widget;

   scene_widget.show();

   return
      QApplication::exec();
}
