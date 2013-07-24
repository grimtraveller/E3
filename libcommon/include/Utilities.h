
//---------------------------------------------------------------------
// Utilities.h
//
// A collection of common tools
//---------------------------------------------------------------------

#include <string>


namespace e3 { namespace common {


// Initializes the random generator with current time.
//
extern void initRandom();

// Generates a random string.
// @param length    length of generated string
// @param charset   chars from which the string will be generated
//
extern std::string randomString(int length, 
    const std::string& charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");

// Platform independent sleep.
//
extern void sleep(unsigned milliseconds);


}} // namespace e3::common