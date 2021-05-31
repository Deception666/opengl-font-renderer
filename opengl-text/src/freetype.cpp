#include "freetype.h"
#include "error_reporting_private.h"

#include <algorithm>
#if __linux__
#include <cstdlib>
#include <filesystem>
#endif
#include <stdexcept>
#include <utility>

#if _WIN32
#include <windows.h>
#elif __linux__
#include <dlfcn.h>
#endif

namespace opengl
{

template < typename T >
inline T GetFuncAddress(
   const char * const function,
   void * const module,
   T ) noexcept
{
#if _WIN32
   return
      reinterpret_cast< T >(
         GetProcAddress(
            reinterpret_cast< const HMODULE >(module),
            function));
#elif __linux__
   return
      reinterpret_cast< T >(
         dlsym(
            module,
            function));
#else
#error "Define for this platform!"
#endif
}

template < size_t OFFSET, typename T >
T GetBitmapData(
   const FT_Face::pointer ft_face ) noexcept
{
#if _WIN32

#if _M_IX86
   const size_t glyph_offset { 84 };
   const size_t bitmap_offset { 76 };
#elif _M_X64
   const size_t glyph_offset { 120 };
   const size_t bitmap_offset { 104 };
#else
#define "Define for this platform type!"
#endif // _M_IX86

#elif __linux__

#if __i386__
   const size_t glyph_offset { 84 };
   const size_t bitmap_offset { 76 };
#elif __x86_64__
   const size_t glyph_offset { 152 };
   const size_t bitmap_offset { 152 };
#else
#define "Define for this platform type!"
#endif // __i386__ 

#else
#error "Define for this platform type!"
#endif // _WIN32

   const uint8_t * const glyph =
      reinterpret_cast< const uint8_t * >(
         *reinterpret_cast< const size_t * >(
            reinterpret_cast< const uint8_t * >(
               ft_face) + glyph_offset));

   return
      *reinterpret_cast< const T * >(
         glyph + bitmap_offset + OFFSET);
}

template < size_t OFFSET, typename T >
T GetGlyphData(
   const FT_Face::pointer ft_face ) noexcept
{
#if _WIN32

#if _M_IX86
   const size_t glyph_offset { 84 };
#elif _M_X64
   const size_t glyph_offset { 120 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

#elif __linux__

#if __i386__
   const size_t glyph_offset { 84 };
#elif __x86_64__
   const size_t glyph_offset { 152 };
#else
#error "Define for this platform type!"
#endif // __i386__

#else
#error "Define for this platform type!"
#endif // _WIN32

   const uint8_t * const glyph =
      reinterpret_cast< const uint8_t * >(
         *reinterpret_cast< const size_t * >(
            reinterpret_cast< const uint8_t * >(
               ft_face) + glyph_offset));

   return
      *reinterpret_cast< const T * >(
         glyph + OFFSET);
}

template < size_t OFFSET, typename T >
T GetFaceData(
   const FT_Face::pointer ft_face ) noexcept
{
   const uint8_t * const face =
      reinterpret_cast< const uint8_t * >(
         ft_face);

   return
      *reinterpret_cast< const T * >(
         face + OFFSET);
}

std::shared_ptr< FreeType >
FreeType::Create( ) noexcept
{
   std::shared_ptr< FreeType >
      instance;

#if _WIN32 || __linux__

   const char * const module_name =

#if _WIN32

#ifdef NDEBUG
      "freetype.dll";
#else
      "freetyped.dll";
#endif

   const auto module =
      LoadLibrary(
         module_name);

#elif __linux__

#ifdef NDEBUG
      "libfreetype.so.6";
#else
      "libfreetype.so.6";
#endif

   const auto module =
      dlopen(
         module_name,
         RTLD_NOW);

#endif

   try
   {
      if (!module)
      {
         throw
            std::runtime_error {
               "Unable to open freetype library " +
               std::string { module_name } };
      }
      else
      {
         instance.reset(
            new FreeType {
               FT_Module {
                  module,
                  [ ] (
                     void * const module )
                  {
#if _WIN32
                     FreeLibrary(
                        static_cast< HMODULE >(
                           module));
#elif __linux__
                     dlclose(
                        module);
#endif
                  }
               }});
      }
   }
   catch (const std::exception & e)
   {
      ReportError(
         e.what());
   }

#else

#error "Define for this platform!"

#endif // _WIN32

   return
      instance;
}

#if __linux__

std::string ConstructFontPath(
   const std::string & font_filename ) noexcept
{
   std::string font_abs_path;

   const auto home_dir =
      std::getenv(
         "HOME");

   const std::string root_dirs[] {
      home_dir ?
         std::string { home_dir } + "/.fonts" :
         std::string { },
      "/usr/local/share/fonts",
      "/usr/share/fonts"
   };

   for (const auto root_dir : root_dirs)
   {
      try
      {
         for (const auto & dir_entry :
              std::filesystem::recursive_directory_iterator(
                 root_dir))
         {
               if (dir_entry.is_regular_file() &&
                   dir_entry.path().filename() == font_filename)
               {
                  font_abs_path =
                     dir_entry.path();

                  break;
               }
         }
      }
      catch (const std::exception & e)
      {
         ReportError(
            e.what());
      }
   }

   return font_abs_path;
}

#endif // __linux__

bool FreeType::SetFont(
   const std::string & font_filename ) noexcept
{
   bool set { false };

#if _WIN32
   const std::string font_abs_path =
      "c:/windows/fonts/" +
      font_filename;
#elif __linux__
   const std::string font_abs_path =
      ConstructFontPath(
         font_filename);
#else
#error "Define for this platform!"
#endif // _WIN32

   FT_Face::pointer face_ptr { nullptr };

   const auto face_created =
      new_face_(
         instance_.get(),
         font_abs_path.c_str(),
         0,
         &face_ptr);

   if (face_created == 0)
   {
      FT_Face face {
         face_ptr,
         face_.get_deleter()
      };

      if (SetSize(face.get(), size_))
      {
         face_.reset(
            face.release());

         glyphs_.clear();

         font_filename_ = font_filename;

         set = true;
      }
   }

   return set;
}

const std::string & FreeType::GetFont( ) const noexcept
{
   return
      font_filename_;
}

bool FreeType::SetSize(
   const uint32_t size ) noexcept
{
   bool set { false };

   if (face_)
   {
      set =
         SetSize(
            face_.get(),
            size);

      if (set)
      {
         size_ = size;

         glyphs_.clear();
      }
   }

   return set;
}

uint32_t FreeType::GetSize( ) const noexcept
{
   return size_;
}

const FreeType::Glyph * FreeType::GetGlyph(
   const uint32_t character ) noexcept
{
   const Glyph * glyph_ptr { nullptr };

   if (face_)
   {
      decltype(glyphs_)::const_iterator glyph_it =
         glyphs_.find(
            character);

      if (glyph_it != glyphs_.cend())
      {
         glyph_ptr =
            &glyph_it->second;
      }
      else
      {
         const int32_t FT_LOAD_RENDER { 1 << 2 };

         const bool loaded =
            load_char_(
               face_.get(),
               character,
               FT_LOAD_RENDER) == 0;

         if (loaded)
         {
            const auto bitmap_width =
               GetBitmapWidth(face_.get());
            const auto bitmap_height =
               GetBitmapHeight(face_.get());

            Glyph glyph {
               bitmap_width,
               bitmap_height,
               GetGlyphTop(face_.get()),
               GetGlyphLeft(face_.get()),
               GetGlyphAdvance(face_.get()),
               std::vector< uint8_t > (
                  bitmap_width * bitmap_height )
            };

            if (const auto data =
                GetBitmapData(face_.get()))
            {
               std::copy(
                  data,
                  data + glyph.bitmap.size(),
                  glyph.bitmap.data());
            }

            const auto inserted =
               glyphs_.emplace(
                  character,
                  std::move(glyph));

            if (inserted.second)
            {
               glyph_ptr =
                  &inserted.first->second;
            }
         }
      }
   }

   return glyph_ptr;
}

double FreeType::GetGlobalGlyphHeight( ) const noexcept
{
   double global_glyph_height { 0.0 };

   if (face_)
   {
      const auto units_per_em =
         GetFaceUnitsPerEM(face_.get());

      if (units_per_em)
      {
         const auto ascender =
            GetFaceAscender(face_.get());
         const auto descender =
            GetFaceDescender(face_.get());

         global_glyph_height =
            static_cast< double >(ascender - descender) *
            GetSize() / units_per_em;
      }
   }

   return
      global_glyph_height;
}

FreeType::FreeType(
   FT_Module module ) :
init_ { GetFuncAddress("FT_Init_FreeType", module.get(), init_) },
uninit_ { GetFuncAddress("FT_Done_FreeType", module.get(), uninit_) },
version_ { GetFuncAddress("FT_Library_Version", module.get(), version_) },
new_face_ { GetFuncAddress("FT_New_Face", module.get(), new_face_) },
delete_face_ { GetFuncAddress("FT_Done_Face", module.get(), delete_face_) },
set_pixel_sizes_ { GetFuncAddress("FT_Set_Pixel_Sizes", module.get(), set_pixel_sizes_) },
load_char_ { GetFuncAddress("FT_Load_Char", module.get(), load_char_) },
size_ { 48 },
glyphs_ { 128 },
module_ { std::move(module) },
instance_ { nullptr, uninit_ },
face_ { nullptr, delete_face_ }
{
   if (!init_ || !uninit_ || !version_ ||
       !new_face_ || !delete_face_ ||
       !set_pixel_sizes_ || !load_char_)
   {
      throw
         std::runtime_error {
            "Required FreeType functions not found!"
         };
   }

   FT_Instance::pointer instance { nullptr };

   if (init_(&instance) != 0)
   {
      throw
         std::runtime_error {
            "Unable to initialize a FT instnace!"
         };
   }

   instance_.reset(
      instance);

   int32_t version_mmp[3] { };

   version_(
      instance,
      version_mmp + 0,
      version_mmp + 1,
      version_mmp + 2);

   const uint32_t version {
      static_cast< uint32_t >(version_mmp[0] << 16) |
      static_cast< uint32_t >(version_mmp[1] << 8) |
      static_cast< uint32_t >(version_mmp[2]) };

   // assume that versions greater than the one
   // specified below are compatible with this impl
   if (version < 0x00020A00)
   {
      throw
         std::runtime_error {
            "FreeType version must be 2.10.4 or greater!"
         };
   }
   else if (version >= 0x00030000)
   {
      throw
         std::runtime_error {
            "FreeType version validated with 2.x.x of the library!  "
            "A major version upgrade may no longer be compatible!"
         };
   }
}

bool FreeType::SetSize(
   const FT_Face::pointer face,
   const uint32_t size ) noexcept
{
   bool set { false };

   if (face)
   {
      set =
         set_pixel_sizes_(
            face,
            0,
            size) == 0;
   }

   return set;
}

uint32_t FreeType::GetBitmapWidth(
   const FT_Face::pointer face ) const noexcept
{
#if _WIN32

#if _M_IX86
   const size_t BITMAP_WIDTH_OFFSET { 4 };
#elif _M_X64
   const size_t BITMAP_WIDTH_OFFSET { 4 };
#else
   #error "Define for this platform type!"
#endif // _M_IX86

#elif __linux__

#if __i386__
   const size_t BITMAP_WIDTH_OFFSET { 4 };
#elif __x86_64__
   const size_t BITMAP_WIDTH_OFFSET { 4 };
#else
   #error "Define for this platform type!"
#endif // __i386__

#else
#error "Define for this platform type!"
#endif // _WIN32

   return
      face ?
      opengl::GetBitmapData<
         BITMAP_WIDTH_OFFSET,
         uint32_t >(
            face) :
      0ul;
}

uint32_t FreeType::GetBitmapHeight(
   const FT_Face::pointer face ) const noexcept
{
#if _WIN32

#if _M_IX86
   const size_t BITMAP_HEIGHT_OFFSET { 0 };
#elif _M_X64
   const size_t BITMAP_HEIGHT_OFFSET { 0 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

#elif __linux__

#if __i386__
   const size_t BITMAP_HEIGHT_OFFSET { 0 };
#elif __x86_64__
   const size_t BITMAP_HEIGHT_OFFSET { 0 };
#else
#error "Define for this platform type!"
#endif // __i386__

#else
#error "Define for this platform type!"
#endif // _WIN32

   return
      face ?
      opengl::GetBitmapData<
         BITMAP_HEIGHT_OFFSET,
         uint32_t >(
            face) :
      0ul;
}

const uint8_t * FreeType::GetBitmapData(
   const FT_Face::pointer face ) const noexcept
{
#if _WIN32

#if _M_IX86
   const size_t BITMAP_DATA_OFFSET { 12 };
#elif _M_X64
   const size_t BITMAP_DATA_OFFSET { 16 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

#elif __linux__

#if __i386__
   const size_t BITMAP_DATA_OFFSET { 12 };
#elif __x86_64__
   const size_t BITMAP_DATA_OFFSET { 16 };
#else
#error "Define for this platform type!"
#endif // __i386__

#else
#error "Define for this platform type!"
#endif // _WIN32

   return
      face ?
      opengl::GetBitmapData<
         BITMAP_DATA_OFFSET,
         const uint8_t * >(
            face) :
      nullptr;
}

int32_t FreeType::GetGlyphTop(
   const FT_Face::pointer face ) const noexcept
{
#if _WIN32

#if _M_IX86
   const size_t GLYPH_TOP_OFFSET { 104 };
#elif _M_X64
   const size_t GLYPH_TOP_OFFSET { 148 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

#elif __linux__

#if __i386__
   const size_t GLYPH_TOP_OFFSET { 104 };
#elif __x86_64__
   const size_t GLYPH_TOP_OFFSET { 196 };
#else
#error "Define for this platform type!"
#endif // __i386__

#else
#error "Define for this platform type!"
#endif // _WIN32

   return
      face ?
      opengl::GetGlyphData<
         GLYPH_TOP_OFFSET,
         int32_t >(
            face) :
      0;
}

int32_t FreeType::GetGlyphLeft(
   const FT_Face::pointer face ) const noexcept
{
#if _WIN32

#if _M_IX86
   const size_t GLYPH_LEFT_OFFSET { 100 };
#elif _M_X64
   const size_t GLYPH_LEFT_OFFSET { 144 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

#elif __linux__

#if __i386__
   const size_t GLYPH_LEFT_OFFSET { 100 };
#elif __x86_64__
   const size_t GLYPH_LEFT_OFFSET { 192 };
#else
#error "Define for this platform type!"
#endif // __i386__

#else
#error "Define for this platform type!"
#endif // _WIN32

   return
      face ?
      opengl::GetGlyphData<
         GLYPH_LEFT_OFFSET,
         int32_t >(
            face) :
      0;
}

double FreeType::GetGlyphAdvance(
   const FT_Face::pointer face ) const noexcept
{
#if _WIN32

#if _M_IX86
   const size_t GLYPH_ADVANCE_OFFSET { 64 };
#elif _M_X64
   const size_t GLYPH_ADVANCE_OFFSET { 88 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

#elif __linux__

#if __i386__
   const size_t GLYPH_ADVANCE_OFFSET { 64 };
#elif __x86_64__
   const size_t GLYPH_ADVANCE_OFFSET { 128 };
#else
#error "Define for this platform type!"
#endif // __i386__

#else
#error "Define for this platform type!"
#endif // _WIN32

   return
      face ?
      opengl::GetGlyphData<
         GLYPH_ADVANCE_OFFSET,
         int32_t >(
            face) / 64.0 :
      0.0;
}

uint16_t FreeType::GetFaceUnitsPerEM(
   const FT_Face::pointer face ) const noexcept
{
#if _WIN32

#if _M_IX86
   const size_t FACE_UNITS_PER_EM_OFFSET { 68 };
#elif _M_X64
   const size_t FACE_UNITS_PER_EM_OFFSET { 104 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

#elif __linux__

#if __i386__
   const size_t FACE_UNITS_PER_EM_OFFSET { 68 };
#elif __x86_64__
   const size_t FACE_UNITS_PER_EM_OFFSET { 136 };
#else
#error "Define for this platform type!"
#endif // __i386__

#else
#error "Define for this platform type!"
#endif // _WIN32

   return
      face ?
      opengl::GetFaceData<
         FACE_UNITS_PER_EM_OFFSET,
         uint16_t >(
            face) :
      0;
}

int16_t FreeType::GetFaceAscender(
   const FT_Face::pointer face ) const noexcept
{
#if _WIN32

#if _M_IX86
   const size_t FACE_ASCENDER_OFFSET { 70 };
#elif _M_X64
   const size_t FACE_ASCENDER_OFFSET { 106 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

#elif __linux__

#if __i386__
   const size_t FACE_ASCENDER_OFFSET { 70 };
#elif __x86_64__
   const size_t FACE_ASCENDER_OFFSET { 138 };
#else
#error "Define for this platform type!"
#endif // __i386__

#else
#error "Define for this platform type!"
#endif // _WIN32

   return
      face ?
      opengl::GetFaceData<
         FACE_ASCENDER_OFFSET,
         int16_t >(
            face) :
      0;
}

int16_t FreeType::GetFaceDescender(
   const FT_Face::pointer face ) const noexcept
{
#if _WIN32

#if _M_IX86
   const size_t FACE_DESCENDER_OFFSET { 72 };
#elif _M_X64
   const size_t FACE_DESCENDER_OFFSET { 108 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

#elif __linux__

#if __i386__
   const size_t FACE_DESCENDER_OFFSET { 72 };
#elif __x86_64__
   const size_t FACE_DESCENDER_OFFSET { 140 };
#else
#error "Define for this platform type!"
#endif // __i386__

#else
#error "Define for this platform type!"
#endif // _WIN32

   return
      face ?
      opengl::GetFaceData<
         FACE_DESCENDER_OFFSET,
         int16_t >(
            face) :
      0;
}

} // namespace opengl
