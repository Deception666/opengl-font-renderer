#ifndef _OPENGL_BOUNDING_BOX_H_
#define _OPENGL_BOUNDING_BOX_H_

#include "library_export.h"

#include <tuple>

namespace opengl
{

class OGL_TEXT_EXPORT BoundingBox final
{
public:
   static BoundingBox InitInvalid( ) noexcept;

   BoundingBox( ) noexcept;
   BoundingBox(
      const std::tuple< float, float, float > & upper_left_front,
      const std::tuple< float, float, float > & lower_right_front,
      const std::tuple< float, float, float > & lower_right_back ) noexcept;

   void Expand(
      const float (&xyz)[3] ) noexcept;
   void Expand(
      const std::tuple< float, float, float > & point ) noexcept;

   BoundingBox Translate(
      const float (&xyz)[3] ) const noexcept;
   BoundingBox Translate(
      const std::tuple< float, float, float > & xyz ) const noexcept;

   void Scale(
      const float scale ) noexcept;

   const std::tuple< float, float, float > &
   GetUpperLeftFront( ) const noexcept;
   const std::tuple< float, float, float > &
   GetLowerRightFront( ) const noexcept;
   const std::tuple< float, float, float > &
   GetLowerRightBack( ) const noexcept;

   float GetWidth( ) const noexcept;
   float GetHeight( ) const noexcept;
   float GetDepth( ) const noexcept;

private:
   std::tuple< float, float, float >
      upper_left_front_;
   std::tuple< float, float, float >
      lower_right_front_;
   std::tuple< float, float, float >
      lower_right_back_;

};

} // namespace opengl

#endif // _OPENGL_BOUNDING_BOX_H_
