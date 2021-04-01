#ifndef _OPENGL_FONT_ENGINE_FREETYPE_H_
#define _OPENGL_FONT_ENGINE_FREETYPE_H_

#include "font_engine.h"

#include <memory>

namespace opengl
{

class FontEngineFreeType final :
   public FontEngine
{
public:
   virtual ~FontEngineFreeType( ) noexcept;

   virtual bool Initialize( ) noexcept override;

private:
   std::shared_ptr< void >
      module_;

};

} // namespace opengl

#endif // _OPENGL_FONT_ENGINE_FREETYPE_H_
