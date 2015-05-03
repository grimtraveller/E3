
//-----------------------------------------------------------------------------------
// e3_Trace.cpp
//
// Writes string messages in printf-style to std:err or ::OutputDebugString
//-----------------------------------------------------------------------------------

#include <stdarg.h>
#include <iostream>
#include <sstream>

#ifdef _WIN32
    #include <windows.h>
    #include <tchar.h>
#endif

#include <e3_Utilities.h>
#include <e3_Trace.h>


namespace e3 { namespace common {

    // Composes and prints the given string to cerr or to some platform specific output device.
    // Format string uses the same syntax as printf. Maximal length for the composed string is 1000.
    // If the composed string exceeds the maximal length, it is truncated.
    // @Return -1 if the composed string is longer than the maximal length - 1, 0 if shorter.
    //
    void trace(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        std::string message;
        vformat(message, format, args);
        va_end(args);

// TODO replace ::OutputDebugString for Mac
#ifdef _CONSOLE
        cerr << s;
#else 
#ifdef _WIN32
        std::basic_ostringstream< TCHAR > os;
        os << message.c_str();
        ::OutputDebugString(os.str().c_str());
#endif
#endif
    }

}} // namespace e3::common

