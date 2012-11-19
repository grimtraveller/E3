

#include <stdarg.h>
#include <iostream>
#include <sstream>


#ifdef __OS_WINDOWS__
	#include <windows.h>
	#include <tchar.h>
#endif

#include <Trace.h>


void Trace::trace(const char* f, ...)
{
	char s[1000];
	va_list marker;
	va_start( marker, f );
	vsprintf( s, f, marker );

	std::cerr << s;

#ifdef __OS_WINDOWS__
	std::basic_ostringstream< TCHAR > os;
	os << s;
	::OutputDebugString(os.str().c_str());
#endif
}
