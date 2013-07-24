
//-----------------------------------------------------------------------------------
// Trace.cpp
//
// A class that outputs string messages in printf-style
//-----------------------------------------------------------------------------------

#include <stdarg.h>
#include <iostream>
#include <sstream>

#ifdef __OS_WINDOWS__
	#include <windows.h>
	#include <tchar.h>
#endif

#include <Trace.h>



namespace e3 { namespace common {

void Tracer::trace(const char* f, ...)
{
	char s[1000];
	va_list args;
	va_start( args, f );
	vsprintf( s, f, args );
    va_end( args );

	std::cerr << s;

#ifdef __OS_WINDOWS__
	std::basic_ostringstream< TCHAR > os;
	os << s;
	::OutputDebugString(os.str().c_str());
#endif
}

}} // namespace e3::common
