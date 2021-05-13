#ifndef _OPENGL_REVERSE_LOCK_H_
#define _OPENGL_REVERSE_LOCK_H_

#include <cstdint>
#include <type_traits>

namespace opengl
{

namespace details
{

template < typename T >
struct has_lock_unlock
{
private:
   using one = uint8_t;
   using two = uint16_t;

   template < typename C > static one test_lock( decltype(&C::lock) );
   template < typename C > static one test_unlock( decltype(&C::unlock) );
   template < typename C > static two test_lock( ... );
   template < typename C > static two test_unlock( ... );

public:
   static const bool value =
      sizeof(test_lock< T >(nullptr)) == sizeof(one) &&
      sizeof(test_unlock< T >(nullptr)) == sizeof(one);
   
};

template < typename T >
inline constexpr bool has_lock_unlock_v =
   has_lock_unlock< T >::value;

} // namespace details

template < typename M >
class reverse_lock final
{
   static_assert(
      details::has_lock_unlock_v< M >,
      "Must have lock / unlock and be of a mutex type!");

public:
   reverse_lock( M & m ) noexcept :
   m_ { m }
   { m_.unlock(); }

   ~reverse_lock( ) noexcept
   { m_.lock(); }

   reverse_lock(
      reverse_lock && ) noexcept = delete;
   reverse_lock(
      const reverse_lock & ) noexcept = delete;

   reverse_lock & operator = (
      reverse_lock && ) noexcept = delete;
   reverse_lock & operator = (
      const reverse_lock & ) noexcept = delete;

private:
   M & m_;

};

} // namespace opengl

#endif // _OPENGL_REVERSE_LOCK_H_
