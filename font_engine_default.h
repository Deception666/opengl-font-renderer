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

   virtual bool SetFont(
      const std::string & font_filename ) noexcept override;
   virtual bool SetFont(
      const std::string & font_filename,
      const uint32_t size ) noexcept override;
   virtual std::string GetFont( ) const noexcept override;

   virtual bool SetSize(
      const uint32_t size ) noexcept override;
   virtual uint32_t GetSize( ) const noexcept override;

   virtual uint32_t GetGlyphMaxWidth( ) const noexcept override;
   virtual uint32_t GetGlyphMaxHeight( ) const noexcept override;
   virtual int32_t GetGlyphMaxTop( ) const noexcept override;
   virtual uint32_t GetLineHeight( ) const noexcept override;

   virtual const Metric * GetGlyphMetric(
      const uint32_t character ) noexcept override;

   virtual TextureMap GetGlyphTextureMap( ) noexcept override;

};

} // namespace opengl

#endif // _OPENGL_FONT_ENGINE_DEFAULT_H_
