
//-----------------------------------------------------------------------------------------
// e3_Exception.h
// Conveniance macros that throw exceptions with infos about source file, function and line number
// @author Tom Buettner
//-----------------------------------------------------------------------------------------

#pragma once

#include <string>
#include <e3_CommonMacros.h>

namespace e3 {

    extern void composeThrowMessage(
        std::string& result,
        const char* exceptionType,
        const char* message,
        const char* filename,
        const char* function,
        int linenumber,
        ...);


    // Throws an object of the given type. 
    // The object must have a constructor with the signature classname(const char*),
    // normally a std::exception.
    // The message for the exception can be formatted in printf-style.
    // In debug mode information about exception type, source file name, 
    // function name and line number where the exception was thrown is injected to the message.
    //
    #define THROW( type, message, ...)                                              \
    {                                                                               \
        std::string s;                                                              \
        e3::composeThrowMessage(                                                    \
            s, #type, message, __FILE_ONLY__, __FUNCTION__, __LINE__, __VA_ARGS__); \
        throw type(s.c_str());                                                      \
    }  


    class Assertion : public std::exception
    {
    public:
        explicit Assertion(const char* message) :
            std::exception(message)
        {}

        virtual ~Assertion() throw() {}
    };



// Throws an e3::Assertion if condition fails.
//
//#define VERIFY( x ) if( (x)==false ) { THROW( e3::Assertion, "Assertion failed: %s", #x ); }
#define VERIFY( x ) FORCE_SEMICOLON (if(! (x)) { THROW( e3::Assertion, "Assertion failed: %s", #x ); })


#ifdef _DEBUG 
#define ASSERT( x ) VERIFY( x )
#else 
#define ASSERT( x )
#endif

} // namespace e3