
//-------------------------------------------------
// CommonMacros.h
//
// A collection of useful macros
//-------------------------------------------------


#pragma once

//-------------------------------------------------
// General 
//-------------------------------------------------

#include <stdexcept>


#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif


//----------------------------------------------------------------
// Macro definitions
//----------------------------------------------------------------

// Strips the path off the __FILE__ macro
//
#define __FILE_ONLY__ \
(strrchr(__FILE__,'\\') \
? strrchr(__FILE__,'\\')+1 \
: __FILE__ \
)


// Throws an exception of the given type.
// The text for the exception can be formatted in printf-style.
//
#ifdef _DEBUG
#define EXCEPTION( exception, message, ...)                                 \
    {                                                                       \
	char fmt[300];                                                          \
	char buf[300];                                                          \
	strcpy( fmt, "[" );                                                     \
	strcat( fmt, #exception );                                              \
	strcat( fmt, " in %s, %s, line %d]\n" );                                \
    strcat( fmt, message );                                                 \
    strcat( fmt, "\n");                                                     \
	sprintf( buf, fmt, __FILE_ONLY__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    throw exception( buf );                                                 \
    }
#else
#define EXCEPTION( exception, message, ...)     \
    {                                           \
	char fmt[300];                              \
	char buf[300];                              \
    strcpy( fmt, message );                     \
    strcat( fmt, "\n");                         \
	sprintf( buf, fmt, __VA_ARGS__ );           \
    throw exception( buf );                     \
    }
#endif

// Throws a std::runtime_error if condition fails.
//
#define VERIFY( x ) if( x==false ) { EXCEPTION( std::runtime_error, "Assertion failed: %s", #x ); }

#ifdef _DEBUG 
#define ASSERT( x ) VERIFY( x )
#else 
#define ASSERT( x )
#endif


// Calculates number of elements in array.
//
#define ARRAY_LENGTH( x ) (sizeof( x ) / sizeof(*( x )))	

// Returns the name of the given type as string.
//
#define TYPENAME( t ) #t	



// Declares a class as singleton
//
#define DECLARE_SINGLETON( TClass )     \
public:                                 \
    static TClass& instance()           \
    {                                   \
        static TClass _instance;        \
        return _instance;               \
    }                                   \
    private:                            \
        TClass();                       \
        TClass( const TClass& );



// Within a class definition declares a test class as friend
//
#ifdef UNITTEST
#define ASSIST_UNIT_TEST( class__ ) friend class class__##Test
#else
#define ASSIST_UNIT_TEST( class__ )
#endif
