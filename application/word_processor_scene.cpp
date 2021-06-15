#include "word_processor_scene.h"

#include <opengl-text/text.h>

#include <QtGui/QKeyEvent>

#include <QtCore/QString>

#include <Qt>

#if _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <algorithm>
#include <exception>
#include <string>
#include <regex>
#include <utility>

WordProcessorScene::WordProcessorScene( ) noexcept :
current_width_ { },
current_height_ { }
{
}

WordProcessorScene::~WordProcessorScene( ) noexcept
{
   try
   {
      text_ = nullptr;
   }
   catch (const std::exception &)
   {
   }
}

bool WordProcessorScene::Initialize( ) noexcept
{
   try
   {
      text_ =
         std::make_unique< opengl::Text >();

      text_->SetText(
         "Font: " + text_->GetFont() + "\n"
         "Size: " + std::to_string(text_->GetFontSize()) + "\n"
         "Scale: " + std::to_string(text_->GetScale()) + "\n");
   }
   catch (const std::exception &)
   {
   }

   return
      text_ != nullptr;
}

bool WordProcessorScene::SetFont(
   const char * const font_filename,
   const uint32_t font_size ) noexcept
{
   bool set { false };

   if (text_)
   {
      set =
         text_->SetFont(
            font_filename,
            font_size);

      if (set)
      {
         const auto & text =
            text_->GetText();

         const std::regex regex {
            R"((?:.*\n){3}((?:.|\n)*))"
         };

         std::smatch matches;

         std::regex_match(
            text,
            matches,
            regex);

         text_->SetText(
            "Font: " + text_->GetFont() + "\n"
            "Size: " + std::to_string(text_->GetFontSize()) + "\n"
            "Scale: " + std::to_string(text_->GetScale()) + "\n" +
            (!matches.empty() ? matches[1].str() : std::string { }));
      }
   }

   return set;
}

bool WordProcessorScene::Resize(
   const uint32_t width,
   const uint32_t height ) noexcept
{
   current_width_ = width;
   current_height_ = height;

   glMatrixMode(
      GL_PROJECTION);

   glLoadIdentity();
   glOrtho(
      0.0, width - 1,
      0.0, height - 1,
      -1.0, 1.0);

   glMatrixMode(
      GL_MODELVIEW);

   glLoadIdentity();

   return
      RepositionText();
}

bool WordProcessorScene::KeyPressed(
   const QKeyEvent & event ) noexcept
{
   bool handled { false };

   if (text_)
   {
      const auto ctrl_alt_modifiers =
         Qt::KeyboardModifier::AltModifier |
         Qt::KeyboardModifier::ControlModifier;

      if ((event.modifiers() & ctrl_alt_modifiers) == ctrl_alt_modifiers &&
          (event.key() == Qt::Key::Key_BracketLeft ||
           event.key() == Qt::Key::Key_BracketRight))
      {
         if (event.key() == Qt::Key::Key_BracketLeft)
         {
            const float scale =
               std::max(
                  text_->GetScale() - 0.025f,
                  0.025f);

            handled =
               text_->SetScale(
                  scale);
         }
         else
         {
            handled =
               text_->SetScale(
                  text_->GetScale() + 0.025f);
         }

         if (handled)
         {
            SetFont(
               text_->GetFont().c_str(),
               text_->GetFontSize());
         }
      }
      else if ((event.modifiers() & Qt::KeyboardModifier::ControlModifier) ==
               Qt::KeyboardModifier::ControlModifier &&
               Qt::Key::Key_1 <= event.key() && event.key() <= Qt::Key::Key_6)
      {
         auto alignment =
            text_->GetAlignment();

         switch (event.key())
         {
         case Qt::Key::Key_1: alignment = alignment & 0xF0 | opengl::Text::Align::LEFT; break;
         case Qt::Key::Key_2: alignment = alignment & 0xF0 | opengl::Text::Align::HCENTER; break;
         case Qt::Key::Key_3: alignment = alignment & 0xF0 | opengl::Text::Align::RIGHT; break;
         case Qt::Key::Key_4: alignment = alignment & 0x0F | opengl::Text::Align::TOP; break;
         case Qt::Key::Key_5: alignment = alignment & 0x0F | opengl::Text::Align::VCENTER; break;
         case Qt::Key::Key_6: alignment = alignment & 0x0F | opengl::Text::Align::BOTTOM; break;
         }

         handled =
            text_->SetAlignment(
               alignment) &&
            RepositionText();
      }
      else if (event.key() == Qt::Key::Key_Return)
      {
         handled =
            text_->AppendChar('\n');
      }
      else if (event.key() == Qt::Key::Key_Backspace)
      {
         const auto & text =
            text_->GetText();

         const std::regex regex {
            R"(((?:.*\n){3})(?:.|\n)*)"
         };

         std::smatch matches;

         std::regex_match(
            text,
            matches,
            regex);

         if (!matches.empty())
         {
            const auto match_size {
               static_cast< size_t >(
                  matches[1].length())
            };

            if (text.length() > match_size)
            {
               auto temp_text =
                  text_->GetText();

               temp_text.pop_back();

               text_->SetText(
                  std::move(temp_text));
            }
         }

         handled = true;
      }
      else
      {
         handled =
            text_->AppendText(
               event.text().toStdString());
      }

      if (handled)
      {
         emit
            Redraw();
      }
   }

   return
      handled;
}

bool WordProcessorScene::Render( ) noexcept
{
   return
      text_ ?
      text_->Render() :
      false;
}

bool WordProcessorScene::RepositionText( ) noexcept
{
   bool set { false };

   if (text_)
   {
      const auto alignment =
         text_->GetAlignment();

      float x_pos { };

      switch (alignment & 0b00001111)
      {
      case opengl::Text::Align::LEFT: x_pos = 0.0f; break;
      case opengl::Text::Align::HCENTER: x_pos = current_width_ / 2.0f; break;
      case opengl::Text::Align::RIGHT: x_pos = current_width_ - 1.0f; break;
      }

      float y_pos { };

      switch (alignment & 0b11110000)
      {
      case opengl::Text::Align::TOP: y_pos = current_height_ - 1.0f; break;
      case opengl::Text::Align::VCENTER: y_pos = current_height_ / 2.0f; break;
      case opengl::Text::Align::BOTTOM: y_pos = 0.0f; break;
      }

      set =
         text_->SetPosition(
            x_pos,
            y_pos,
            0.0f);
   }

   return set;
}
