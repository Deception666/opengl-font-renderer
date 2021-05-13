#include "error_reporting_private.h"
#include "error_reporting.h"

namespace opengl
{

std::function< void ( const char * const ) >
   error_callback_;

void SetErrorCallback(
   std::function< void ( const char * const ) > callback ) noexcept
{
   // only one callback and the user is responsible
   // for any thread safety for setting the callback
   error_callback_.swap(
      callback);
}

void ReportError(
   const char * const error ) noexcept
{
   if (error_callback_ && error)
   {
      // user is responsible for any thread safety for
      // when the error callback is called.  the user
      // must also make sure that no exceptions are
      // thrown in the user's callback.
      error_callback_(
         error);
   }
}

} // namespace opengl
