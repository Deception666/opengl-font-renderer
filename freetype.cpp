#include "freetype.h"

#include <algorithm>
#include <exception>
#include <utility>

#if _WIN32
#include <windows.h>
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
#define "Define for this platform type!"
#elif _M_X64
   const size_t glyph_offset { 120 };
   const size_t bitmap_offset { 104 };
#else
#define "Define for this platform type!"
#endif // _M_IX86

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
#error "Define for this platform type!"
#elif _M_X64
   const size_t glyph_offset { 120 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

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

std::shared_ptr< FreeType >
FreeType::Create( ) noexcept
{
   std::shared_ptr< FreeType >
      instance;

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
      try
      {
         instance.reset(
            new FreeType(
               FT_Module {
                  module,
                  [ ] (
                     void * const module )
                  {
                     FreeLibrary(
                        static_cast< HMODULE >(
                           module));
                  }
               }));
      }
      catch (const std::exception & e)
      {
         // exception thrown
      }
   }

#else

#error "Define for this platform!"

#endif // _WIN32

   return
      instance;
}

bool FreeType::SetFont(
   const std::string & font_filename ) noexcept
{
   bool set { false };

#if _WIN32
   const std::string font_abs_path =
      "c:/windows/fonts/" +
      font_filename;
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

         set = true;
      }
   }

   return set;
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

uint32_t FreeType::GetSize(
   const uint32_t size ) const noexcept
{
   return size_;
}

const FreeType::Glyph * FreeType::GetGlyph(
   const uint32_t character ) noexcept
{
   const Glyph * glyph_ptr { nullptr };

   if (face_)
   {
      decltype(glyphs_)::const_iterator glyph =
         glyphs_.find(
            character);

      if (glyph != glyphs_.cend())
      {
         glyph_ptr =
            &glyph->second;
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
         std::exception {
            "Required FreeType functions not found!"
         };
   }

   FT_Instance::pointer instance { nullptr };

   if (init_(&instance) != 0)
   {
      throw
         std::exception {
            "Unable to initialize a FT instnace!"
         };
   }

   instance_.reset(
      instance);
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
#error "Define for this platform type!"
#elif _M_X64
   const size_t BITMAP_WIDTH_OFFSET { 4 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

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
#error "Define for this platform type!"
#elif _M_X64
   const size_t BITMAP_HEIGHT_OFFSET { 0 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

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
#error "Define for this platform type!"
#elif _M_X64
   const size_t BITMAP_DATA_OFFSET { 16 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

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
#error "Define for this platform type!"
#elif _M_X64
   const size_t GLYPH_TOP_OFFSET { 148 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

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
#error "Define for this platform type!"
#elif _M_X64
   const size_t GLYPH_LEFT_OFFSET { 144 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

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
#error "Define for this platform type!"
#elif _M_X64
   const size_t GLYPH_ADVANCE_OFFSET { 88 };
#else
#error "Define for this platform type!"
#endif // _M_IX86

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

} // namespace opengl