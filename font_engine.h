#ifndef _OPENGL_FONT_ENGINE_H_
#define _OPENGL_FONT_ENGINE_H_

namespace opengl
{

class Font;

class FontEngine
{
public:
   virtual ~FontEngine( ) noexcept = 0 { }

   virtual bool Initialize( ) noexcept = 0;

};

} // namespace opengl

#endif // _OPENGL_FONT_ENGINE_H_
