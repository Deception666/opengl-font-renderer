#ifndef _OPENGL_FONT_ENGINE_DEFAULT_H_
#define _OPENGL_FONT_ENGINE_DEFAULT_H_

#include "font_engine.h"

namespace opengl
{

class FontEngineDefault final :
   public FontEngine
{
public:
   virtual ~FontEngineDefault( ) noexcept;

   virtual bool Initialize( ) noexcept override;

};

} // namespace opengl

#endif // _OPENGL_FONT_ENGINE_DEFAULT_H_
