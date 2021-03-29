#include <QtWidgets/QApplication>
#include <QtWidgets/QOpenGLWidget>

#include <cstdint>

//////////////////////////////////////////////////////////////////
#include <memory>
#include <cstring>
#include <algorithm>

#include <QtGui/QKeyEvent>

//#include <ft2build.h>
//#include <freetype/freetype.h>

using FT_Module___ =
   std::unique_ptr<
      void,
      int32_t (__stdcall *) ( void * ) >;

using FT_Instance = void *;
using FT_Face = void *;

template < typename T >
T GetFuncAddress(
   const char * const function,
   void * const module,
   T ) noexcept
{
   return
      reinterpret_cast< T >(
         GetProcAddress(
            reinterpret_cast< const HMODULE >(module),
            function));
}

struct FreeTypeLibrary
{
   FreeTypeLibrary(
      void * const module ) :
   init { GetFuncAddress("FT_Init_FreeType", module, init) },
   uninit { GetFuncAddress("FT_Done_FreeType", module, uninit) },
   version { GetFuncAddress("FT_Library_Version", module, version) },
   new_face { GetFuncAddress("FT_New_Face", module, new_face) },
   delete_face { GetFuncAddress("FT_Done_Face", module, delete_face) },
   set_pixel_sizes { GetFuncAddress("FT_Set_Pixel_Sizes", module, set_pixel_sizes) },
   load_char { GetFuncAddress("FT_Load_Char", module, load_char) }
   {
   }

   int32_t (* const init) ( FT_Instance * );
   int32_t (* const uninit) ( FT_Instance );
   void (* const version) ( FT_Instance, int32_t *, int32_t *, int32_t * );
   int32_t (* const new_face) ( FT_Instance, const char *, long, FT_Face * );
   int32_t (* const delete_face) ( FT_Face );
   int32_t (* const set_pixel_sizes) ( FT_Face, uint32_t, uint32_t );
   int32_t (* const load_char) ( FT_Face, uint32_t, int32_t );

};

template < size_t OFFSET, typename T >
T GetBitmapData(
   const FT_Face ft_face ) noexcept
{
#if _WIN64
   const size_t glyph_offset { 120 };
   const size_t bitmap_offset { 104 };
#else
#error "Define for this platform type!"
#endif

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
   const FT_Face ft_face ) noexcept
{
#if _WIN64
   const size_t glyph_offset { 120 };
#else
#error "Define for this platform type!"
#endif

   const uint8_t * const glyph =
      reinterpret_cast< const uint8_t * >(
         *reinterpret_cast< const size_t * >(
            reinterpret_cast< const uint8_t * >(
               ft_face) + glyph_offset));

   return
      *reinterpret_cast< const T * >(
         glyph + OFFSET);
}

float scale { 1.0f };
uint32_t glyph_max_height { 0 };
uint32_t size { 48 };
uint32_t tex_width { 2048 }, tex_height { 2048 };
size_t font_index { 0 };
std::unique_ptr< uint8_t [] > texture;

struct glyph
{
   uint32_t width;
   uint32_t height;
   int32_t top;
   int32_t left;
   double advance;
   float uv_ll[2];
   float uv_ur[2];
};

std::unique_ptr< glyph [] > glyphs;

void font_test( )
{
   const FT_Module___ module {
      LoadLibrary("freetyped.dll"),
      reinterpret_cast< int32_t (__stdcall *) ( void * ) >(&FreeLibrary)
   };

   FreeTypeLibrary library {
      module.get()
   };

   FT_Instance ft_instance { nullptr };

   library.init(
      &ft_instance);

   int32_t version[3] { };

   library.version(
      ft_instance,
      &version[0],
      &version[1],
      &version[2]);

   FT_Face ft_face { nullptr };

   const char * const fonts[] {
      "c:/windows/fonts/arial.ttf",
      "c:/windows/fonts/itcedscr.ttf",
      "c:/windows/fonts/bradhitc.ttf",
      "c:/windows/fonts/consola.ttf"
   };

   size_t index =
      std::clamp(
         font_index,
         static_cast< size_t >(0),
         std::size(fonts) - 1);

   library.new_face(
      ft_instance,
      fonts[index],
      0,
      &ft_face);

   library.set_pixel_sizes(
      ft_face,
      0,
      size);

   glyph_max_height = 0;

   uint32_t current_row { };
   uint32_t current_height { };
   uint32_t next_line { };

   texture.reset(
      new uint8_t[tex_width * tex_height] { });

   glyphs.reset(
      new glyph[128] { });

   for (uint32_t i { 32 }; 127u > i; ++i)
   {
      library.load_char(
         ft_face,
         i,
         1 << 2);

      glyphs[i].height =
         GetBitmapData< 0, uint32_t >(
            ft_face);
      glyphs[i].width =
         GetBitmapData< 4, uint32_t >(
            ft_face);
      const auto buffer =
         GetBitmapData< 16, const uint8_t * >(
            ft_face);
      glyphs[i].left =
         GetGlyphData< 144, int32_t >(
            ft_face);
      glyphs[i].top =
         GetGlyphData< 148, int32_t >(
            ft_face);
      glyphs[i].advance =
         GetGlyphData< 88, int32_t >(
            ft_face) / 64.0;

      const auto width =
         glyphs[i].width;
      const auto height =
         glyphs[i].height;

      if (current_row + width >= tex_width)
      {
         current_row = 0;
         current_height = next_line + 5;
      }
      
      if (current_height + height >= tex_height)
      {
         break;
      }

      for (uint32_t i { }; height > i; ++i)
      {
         std::copy(
            buffer + width * (height - i - 1),
            buffer + width * (height - i),
            (texture.get() + (current_height + i) * tex_width + current_row));
      }

      glyphs[i].uv_ll[0] =
         static_cast< double >(current_row) /
         static_cast< double >(tex_width - 1);
      glyphs[i].uv_ll[1] =
         static_cast< double >(current_height) /
         static_cast< double >(tex_width - 1);

      glyphs[i].uv_ur[0] =
         static_cast< double >(current_row + width) /
         static_cast< double >(tex_width - 1);
      glyphs[i].uv_ur[1] =
         static_cast< double >(current_height + height) /
         static_cast< double >(tex_width - 1);

      current_row += width + 5;

      next_line =
         std::max(
            next_line,
            current_height + height);

      glyph_max_height =
         std::max(
            glyph_max_height,
            height);
   }

   library.delete_face(
      ft_face);

   library.uninit(
      ft_instance);
}
//////////////////////////////////////////////////////////////////

class OpenGLWidget :
   public QOpenGLWidget
{
public:

   std::string string;
   virtual void keyReleaseEvent(QKeyEvent *event)
   {
      if ((event->key() == Qt::Key::Key_Plus ||
           event->key() == Qt::Key::Key_Minus) &&
           event->modifiers() & Qt::KeyboardModifier::ControlModifier)
      {
         if (event->key() == Qt::Key::Key_Plus)
            ++::size;
         else
            --::size;

         ::size =
            std::clamp(
               ::size,
               1u,
               1000u);

         font_test();

         update();
      }
      else if ((event->key() == Qt::Key::Key_BracketLeft ||
           event->key() == Qt::Key::Key_BracketRight) &&
           event->modifiers() & Qt::KeyboardModifier::ControlModifier)
      {
         if (event->key() == Qt::Key::Key_BracketLeft)
            ::scale -= 0.1f;
         else
            ::scale += 0.1f;

         ::scale =
            std::clamp(
               ::scale,
               0.1f,
               20.0f);

         update();
      }
      else if (!event->text().isEmpty() &&
               '0' <= event->text().at(0) &&
               event->text().at(0) <= '9' &&
               event->modifiers() & Qt::KeyboardModifier::ControlModifier)
      {
         font_index =
            event->text().toInt();

         font_test();
      }
      else if (event->key() == Qt::Key::Key_Return)
      {
         string += '\n';

         update();
      }
      else if (event->key() == Qt::Key::Key_Backspace)
      {
         if (!string.empty())
         {
            string.pop_back();

            update();
         }
      }
      else
      {
         string +=
            event->text().toStdString();

         update();
      }
   }

protected:
   // overrides from QOpenGLWidget
   virtual void initializeGL( ) override;
   virtual void resizeGL(
      const int32_t width,
      const int32_t height ) override;
   virtual void paintGL( ) override;

private:

};

void OpenGLWidget::initializeGL( )
{
}

void OpenGLWidget::resizeGL(
   const int32_t width,
   const int32_t height )
{
}

void RenderText(
   const char * const text,
   const float x,
   const float y,
   const float scale )
{
   const char * s { text };
   const char * const e { text + std::strlen(text) };

   glPushMatrix();

   glScalef(scale, scale, 1.0);
   glTranslatef(x / scale, y / scale, 0);

   glBegin(
      GL_TRIANGLES);

   float pen_x { };
   float pen_y { };

   for (; s != e; ++s)
   {
      const int8_t c { *s };

      const auto & g =
         glyphs[c];

      if (c == '\n')
      {
         pen_x = 0;
         pen_y -= glyph_max_height + 5.0;
      }
      else
      {
         if (c != ' ')
         {
            const float x =
               pen_x + g.left;
            const float y =
               pen_y - (static_cast< float >(g.height) - g.top);

            glTexCoord2f(g.uv_ll[0], g.uv_ur[1]);
            glVertex3f(x, y + g.height, 0.0f);
            glTexCoord2f(g.uv_ll[0], g.uv_ll[1]);
            glVertex3f(x, y, 0.0f);
            glTexCoord2f(g.uv_ur[0], g.uv_ll[1]);
            glVertex3f(x + g.width, y, 0.0f);

            glTexCoord2f(g.uv_ll[0], g.uv_ur[1]);
            glVertex3f(x, y + g.height, 0.0f);
            glTexCoord2f(g.uv_ur[0], g.uv_ll[1]);
            glVertex3f(x + g.width, y, 0.0f);
            glTexCoord2f(g.uv_ur[0], g.uv_ur[1]);
            glVertex3f(x + g.width, y + g.height, 0.0f);
         }

         pen_x += g.advance;
      }
   }

   glEnd();

   glPopMatrix();
}

void OpenGLWidget::paintGL( )
{
   static uint32_t tid { 0 };

   if (texture)
   {
      if (tid)
         glDeleteTextures(
            1,
            &tid);

      glGenTextures(
         1,
         &tid);
      glBindTexture(
         GL_TEXTURE_2D,
         tid);
      glPixelStorei(
         GL_UNPACK_ALIGNMENT,
         1);
      glTexImage2D(
         GL_TEXTURE_2D,
         0,
         GL_LUMINANCE,
         tex_width,
         tex_height,
         0,
         GL_RED,
         GL_UNSIGNED_BYTE,
         texture.get());
      glPixelStorei(
         GL_UNPACK_ALIGNMENT,
         4);
      reinterpret_cast< void (*) ( GLenum ) >(
         wglGetProcAddress("glGenerateMipmap"))(
            GL_TEXTURE_2D);
      glTexParameteri(
         GL_TEXTURE_2D,
         GL_TEXTURE_WRAP_S,
         GL_CLAMP_TO_BORDER);
      glTexParameteri(
         GL_TEXTURE_2D,
         GL_TEXTURE_WRAP_T,
         GL_CLAMP_TO_BORDER);
      glTexParameteri(
         GL_TEXTURE_2D,
         GL_TEXTURE_MIN_FILTER,
         GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(
         GL_TEXTURE_2D,
         GL_TEXTURE_MAG_FILTER,
         GL_LINEAR);
      glBindTexture(
         GL_TEXTURE_2D,
         tid);
      texture.reset();

      assert(glGetError() == GL_NO_ERROR);
   }

   glEnable(
      GL_BLEND);
   glBlendFunc(
      GL_SRC_ALPHA,
      GL_ONE_MINUS_SRC_ALPHA);

   glMatrixMode(
      GL_PROJECTION);
   glLoadIdentity();
   glOrtho(
      0.0, width(),
      0.0, height(),
      -1.0, 0.0);

   glMatrixMode(
      GL_MODELVIEW);
   glLoadIdentity();

   glEnable(
      GL_TEXTURE_2D);
   glBindTexture(
      GL_TEXTURE_2D,
      tid);

   const std::string s {
      "scale: " + std::to_string(scale) + "\n"
      "size: " + std::to_string(::size) + "\n"
      + string
   };

   RenderText(
      s.c_str(),
      0.0f, height() - glyph_max_height * scale,
      scale);

   //glBegin(
   //   GL_TRIANGLES);
   //
   //glTexCoord2f(0.0f, 1.0f);
   //glVertex3f(-1.0f, 1.0f, 0.0);
   //glTexCoord2f(0.0f, 0.0f);
   //glVertex3f(-1.0f, -1.0f, 0.0);
   //glTexCoord2f(1.0f, 0.0f);
   //glVertex3f(1.0f, -1.0f, 0.0);
   //
   //glTexCoord2f(0.0f, 1.0f);
   //glVertex3f(-1.0f, 1.0f, 0.0);
   //glTexCoord2f(1.0f, 0.0f);
   //glVertex3f(1.0f, -1.0f, 0.0);
   //glTexCoord2f(1.0f, 1.0f);
   //glVertex3f(1.0f, 1.0f, 0.0);
   //
   //glEnd();

   glBindTexture(
      GL_TEXTURE_2D,
      0);

   glDisable(
      GL_BLEND);
}

int32_t main(
   const int32_t argc,
   const char * const (&argv)[] )
{
   int32_t argc_ { argc };
   char ** argv_ { const_cast< char ** >(argv) };

   QApplication application {
      argc_,
      argv_
   };

   font_test();

   // add surface format capabilities here

   OpenGLWidget ogl_widget;

   ogl_widget.show();

   return
      QApplication::exec();
}
