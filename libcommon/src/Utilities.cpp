
//---------------------------------------------------------------------
// Utilities.cpp
//
// A collection of common tools
//---------------------------------------------------------------------

#include <cstdlib>
#include <time.h> 

#include <Utilities.h>


namespace e3 { namespace common {


void initRandom()
{
    srand((unsigned)time(NULL));
}



std::string randomString(int length, const std::string& charset)
{
    std::string result = "";
	result.resize(length);

	for(int i = 0; i < length; i++) {
        result[i] = charset[rand() % charset.length()];
	}
	return result;
}


#ifdef __OS_WINDOWS__
#include <windows.h>
void sleep(unsigned milliseconds) {
    Sleep(milliseconds);
}
#elif defined(__OS_POSIX__)
#include <unistd.h>
void sleep(unsigned milliseconds) {
    usleep(milliseconds * 1000);
}
#endif

}} // namespace e3::common