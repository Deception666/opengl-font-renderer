#ifndef _OPENGL_TEXT_TEST_WORD_PROCESSOR_SCENE_H_
#define _OPENGL_TEXT_TEST_WORD_PROCESSOR_SCENE_H_

#include "scene.h"

#include <cstdint>
#include <memory>

namespace opengl
{
class Text;
} // namespace opengl

class WordProcessorScene :
   public Scene
{
public:
   WordProcessorScene( ) noexcept;
   virtual ~WordProcessorScene( ) noexcept;

   WordProcessorScene(
      WordProcessorScene && ) noexcept = delete;
   WordProcessorScene(
      const WordProcessorScene & ) noexcept = delete;

   WordProcessorScene & operator = (
      WordProcessorScene && ) noexcept = delete;
   WordProcessorScene & operator = (
      const WordProcessorScene & ) noexcept = delete;

   virtual bool Initialize( ) noexcept override;

   virtual bool SetFont(
      const char * const font_filename,
      const uint32_t font_size ) noexcept override;

   virtual bool Resize(
      const uint32_t width,
      const uint32_t height ) noexcept override;

   virtual bool KeyPressed(
      const QKeyEvent & event ) noexcept override;

   virtual bool Render( ) noexcept override;

private:
   bool RepositionText( ) noexcept;

   bool TextAttributesUpdated( ) noexcept;

   std::unique_ptr< opengl::Text >
      text_;

   uint32_t current_width_;
   uint32_t current_height_;

};

#endif // _OPENGL_TEXT_TEST_WORD_PROCESSOR_SCENE_H_
