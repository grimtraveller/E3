
#pragma once

#include <stdexcept>

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
#define EXCEPTION( type, message, ...) \
    {\
	char f[300];\
	char b[300];\
	strcpy( f, "[" ); \
	strcat( f, #type );\
	strcat( f, " in %s, %s, line %d]\n" );\
    strcat( f, message );\
    strcat( f, "\n");\
	sprintf( b, f, __FILE_ONLY__, __FUNCTION__, __LINE__, __VA_ARGS__);\
    throw type( b );\
    }
#else
#define EXCEPTION( type, message, ...) \
    {\
	char f[300];\
	char b[300];\
    strcpy( f, message );\
    strcat( f, "\n");\
	sprintf( b, f, __VA_ARGS__ );\
    throw type( b );\
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

