#include "bounding_box.h"

#include <algorithm>
#include <limits>

namespace opengl
{

BoundingBox BoundingBox::InitInvalid( ) noexcept
{
   return {
      {
         std::numeric_limits< float >::max(),
         std::numeric_limits< float >::min(),
         std::numeric_limits< float >::min()
      },
      {
         std::numeric_limits< float >::min(),
         std::numeric_limits< float >::max(),
         std::numeric_limits< float >::max()
      },
      {
         std::numeric_limits< float >::min(),
         std::numeric_limits< float >::max(),
         std::numeric_limits< float >::max()
      }
   };
}

BoundingBox::BoundingBox( ) noexcept :
upper_left_front_ { 0.0f, 0.0f, 0.0f },
lower_right_front_ { 0.0f, 0.0f, 0.0f },
lower_right_back_ { 0.0f, 0.0f, 0.0f }
{
}

BoundingBox::BoundingBox(
   const std::tuple< float, float, float > & upper_left_front,
   const std::tuple< float, float, float > & lower_right_front,
   const std::tuple< float, float, float > & lower_right_back ) noexcept :
upper_left_front_ { upper_left_front },
lower_right_front_ { lower_right_front },
lower_right_back_ { lower_right_back }
{
}

void BoundingBox::Expand(
   const float(&xyz)[3] ) noexcept
{
   std::get< 0 >(upper_left_front_) =
      std::min(
         std::get< 0 >(upper_left_front_),
         xyz[0]);
   std::get< 1 >(upper_left_front_) =
      std::max(
         std::get< 1 >(upper_left_front_),
         xyz[1]);
   std::get< 2 >(upper_left_front_) =
      std::max(
         std::get< 2 >(upper_left_front_),
         xyz[2]);
   
   std::get< 0 >(lower_right_front_) =
      std::max(
         std::get< 0 >(lower_right_front_),
         xyz[0]);
   std::get< 1 >(lower_right_front_) =
      std::min(
         std::get< 1 >(lower_right_front_),
         xyz[1]);
   std::get< 2 >(lower_right_front_) =
      std::get< 2 >(lower_right_front_);

   std::get< 0 >(lower_right_back_) =
      std::get< 0 >(lower_right_front_);
   std::get< 1 >(lower_right_back_) =
      std::get< 1 >(lower_right_front_);
   std::get< 2 >(lower_right_back_) =
      std::min(
         std::get< 2 >(lower_right_back_),
         xyz[2]);
}

void BoundingBox::Expand(
   const std::tuple< float, float, float > & point ) noexcept
{
   const float xyz[] {
      std::get< 0 >(point),
      std::get< 1 >(point),
      std::get< 2 >(point)
   };

   Expand(xyz);
}

BoundingBox BoundingBox::Translate(
   const float (&xyz)[3] ) const noexcept
{
   return {
      {
         std::get< 0 >(upper_left_front_) + xyz[0],
         std::get< 1 >(upper_left_front_) + xyz[1],
         std::get< 2 >(upper_left_front_) + xyz[2]
      },
      {
         std::get< 0 >(lower_right_front_) + xyz[0],
         std::get< 1 >(lower_right_front_) + xyz[1],
         std::get< 2 >(lower_right_front_) + xyz[2]
      },
      {
         std::get< 0 >(lower_right_back_) + xyz[0],
         std::get< 1 >(lower_right_back_) + xyz[1],
         std::get< 2 >(lower_right_back_) + xyz[2]
      }
   };
}

BoundingBox BoundingBox::Translate(
   const std::tuple< float, float, float > & xyz ) const noexcept
{
   const float array_xyz[3] {
      std::get< 0 >(xyz),
      std::get< 1 >(xyz),
      std::get< 2 >(xyz)
   };

   return
      Translate(array_xyz);
}

void BoundingBox::Scale(
   const float scale ) noexcept
{
   std::get< 0 >(upper_left_front_) *= scale;
   std::get< 1 >(upper_left_front_) *= scale;
   std::get< 2 >(upper_left_front_) *= scale;

   std::get< 0 >(lower_right_front_) *= scale;
   std::get< 1 >(lower_right_front_) *= scale;
   std::get< 2 >(lower_right_front_) *= scale;

   std::get< 0 >(lower_right_back_) *= scale;
   std::get< 1 >(lower_right_back_) *= scale;
   std::get< 2 >(lower_right_back_) *= scale;
}

const std::tuple< float, float, float > &
BoundingBox::GetUpperLeftFront( ) const noexcept
{
   return
      upper_left_front_;
}

const std::tuple< float, float, float > &
BoundingBox::GetLowerRightFront( ) const noexcept
{
   return
      lower_right_front_;
}

const std::tuple< float, float, float > &
BoundingBox::GetLowerRightBack( ) const noexcept
{
   return
      lower_right_back_;
}

float BoundingBox::GetWidth( ) const noexcept
{
   return
      std::get< 0 >(lower_right_front_) -
      std::get< 0 >(upper_left_front_);
}

float BoundingBox::GetHeight( ) const noexcept
{
   return
      std::get< 1 >(upper_left_front_) -
      std::get< 1 >(lower_right_front_);
}

float BoundingBox::GetDepth( ) const noexcept
{
   return
      std::get< 2 >(lower_right_front_) -
      std::get< 2 >(lower_right_back_);
}

} // namespace opengl
