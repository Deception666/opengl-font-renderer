#ifndef _OPENGL_ERROR_REPORTING_H_
#define _OPENGL_ERROR_REPORTING_H_

#include "library_export.h"

#include <functional>

namespace opengl
{

void OGL_TEXT_EXPORT SetErrorCallback(
   std::function< void ( const char * const ) > callback ) noexcept;

} // namespace opengl

#endif // _OPENGL_ERROR_REPORTING_H_
