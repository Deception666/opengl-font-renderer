#ifndef _OPENGL_TEXT_TEST_SCENE_H_
#define _OPENGL_TEXT_TEST_SCENE_H_

#include <QObject>

#include <cstdint>

class QKeyEvent;

class Scene :
   public QObject
{
   Q_OBJECT;

signals:
   void Redraw( ) const;

public:
   virtual ~Scene( ) noexcept = default;

   virtual bool Initialize( ) noexcept = 0;

   virtual bool SetFont(
      const char * const font_filename,
      const uint32_t font_size ) noexcept = 0;

   virtual bool Resize(
      const uint32_t width,
      const uint32_t height ) noexcept = 0;

   virtual bool KeyPressed(
      const QKeyEvent & event ) noexcept = 0;

   virtual bool Render( ) noexcept = 0;

};

#endif // _OPENGL_TEXT_TEST_SCENE_H_
