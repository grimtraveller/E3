
//-------------------------------------------------
// e3_CommonMacros.h
//
// A collection of useful macros
//-------------------------------------------------


#pragma once


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


// Calculates number of elements in array.
//
#define ARRAY_SIZE( x ) (sizeof( x ) / sizeof(*( x )))	

// Returns the name of the given type as string.
//
#define TYPENAME( t ) #t	


/** This macro can be applied to a float variable to check whether it contains a denormalised
value, and to normalise it if necessary.
On CPUs that aren't vulnerable to denormalisation problems, this will have no effect.
*/
#define UNDENORMALIZE(x)   x += 1.0f; x -= 1.0f;




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


#if _MSC_VER
#define FORCE_SEMICOLON(x) \
   __pragma(warning(push)) \
   __pragma(warning(disable:4127)) \
   do { x } while (false) \
   __pragma(warning(pop))
#else
/** This is the good old C++ trick for creating a macro that forces the user to put
a semicolon after it when they use it.
*/
#define FORCE_SEMICOLON(x) do { x } while (false)
#endif

//#define UNUSED(symbol)  (symbol) = (symbol)
#define UNUSED(symbol)  (void)symbol

#define DUMMY_STATEMENT int xyz = 0; xyz = xyz;