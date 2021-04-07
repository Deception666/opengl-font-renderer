#include "font_engine_factory.h"
#include "font_engine.h"
#include "font_engine_default.h"
#include "font_engine_freetype.h"

namespace opengl {
namespace font_engine_factory {

std::vector<
   std::shared_ptr< FontEngine >::weak_type >
   font_engines_ {
      static_cast< size_t >(FontEngineType::MAX_TYPES),
      std::shared_ptr< FontEngine >::weak_type { } };

std::vector< uint32_t >
   default_character_set_;

std::shared_ptr< FontEngine >
CreateFontEngine(
   const FontEngineType type ) noexcept
{
   std::shared_ptr< FontEngine > font_engine {
      nullptr
   };

   switch (type)
   {
   case FontEngineType::DEFAULT:
      font_engine =
         std::make_shared< FontEngineDefault >();

      break;
   
   case FontEngineType::FREETYPE:
      font_engine =
         std::make_shared< FontEngineFreeType >();
      
      break;
   }

   if (font_engine)
   {
      const bool initialized =
         font_engine->Initialize();

      if (!initialized)
      {
         font_engine.reset();
      }
   }

   return
      font_engine;
}

std::shared_ptr< FontEngine >
ConstructFontEngine(
   const FontEngineType type ) noexcept
{
   std::shared_ptr< FontEngine > font_engine {
      nullptr
   };

   const size_t index =
      static_cast< size_t >(type);

   if (font_engines_.size() > index)
   {
      font_engine =
         font_engines_.at(index).lock();

      if (!font_engine)
      {
         font_engine =
            CreateFontEngine(
               type);
      }

      if (!font_engine &&
          type != FontEngineType::DEFAULT)
      {
         font_engine =
            ConstructFontEngine(
               FontEngineType::DEFAULT);
      }
   }

   return
      font_engine;
}

void SetDefaultCharacterSet(
   std::vector< uint32_t > character_set ) noexcept
{
   default_character_set_.swap(
      character_set);
}

}} // namespace opengl::font_engine_factory
