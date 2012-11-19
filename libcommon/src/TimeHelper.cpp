
#include <TimeHelper.h>

#ifdef __OS_WINDOWS__
#include <windows.h>
void sleep_ms(unsigned milliseconds) {
    Sleep(milliseconds);
}
#elif defined(__OS_POSIX__)
#include <unistd.h>
void sleep_ms(unsigned milliseconds) {
    usleep(milliseconds * 1000);
}
#endif