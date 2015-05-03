

#include <stdexcept>
#include <cstdarg>
#include <sstream>

//#include "e3_CommonMacros.h"
#include "e3_Utilities.h"
#include "e3_Exception.h"

namespace e3 {

#ifdef _DEBUG
    void composeThrowMessage(
        std::string& result,
        const char* type,
        const char* message,
        const char* filename,
        const char* function,
        int linenumber,
        ...)
    {
        std::ostringstream os;
        try {
            os << "[" << type << " in " << filename << ", " << function << ", " << linenumber << "]";
            os << std::endl << message << std::endl;
            va_list args;
            va_start(args, linenumber);
            e3::common::vformat(result, os.str().c_str(), args);
            va_end(args);
        }
        catch (...) {}
    }
#else
    void composeThrowMessage(
        std::string& result,
        const char* type,
        const char* message,
        const char* filename,
        const char* function,
        int linenumber,
        ...)
    {
        std::ostringstream os;
        try {
            os << message << std::endl;
            va_list args;
            va_start(args, linenumber);
            e3::common::vformat(result, os.str().c_str(), args);
            va_end(args);
        }
        catch (...) {}
    }
#endif

} // namespace e3