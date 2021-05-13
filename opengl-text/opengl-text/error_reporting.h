#ifndef _OPENGL_ERROR_REPORTING_H_
#define _OPENGL_ERROR_REPORTING_H_

#include <functional>

namespace opengl
{

void SetErrorCallback(
   std::function< void ( const char * const ) > callback ) noexcept;

} // namespace opengl

#endif // _OPENGL_ERROR_REPORTING_H_
