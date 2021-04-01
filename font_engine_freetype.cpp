#include "font_engine_freetype.h"

#if _WIN32
#include <windows.h>
#endif

namespace opengl
{

FontEngineFreeType::~FontEngineFreeType( ) noexcept
{
}

bool FontEngineFreeType::Initialize( ) noexcept
{
   bool initialized { false };

#if _WIN32

   const char * const module_name =
#ifdef NDEBUG
      "freetype.dll";
#else
      "freetyped.dll";
#endif

   const auto module =
      LoadLibrary(
         module_name);

   if (module)
   {
      module_.reset(
         module,
         &FreeLibrary);

      // get all of the function attribues

      initialized = true;
   }

#endif

   return
      initialized;
}

} // namespace opengl
