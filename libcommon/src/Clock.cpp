
//------------------------------------------------------------
// Clock.cpp
//
// A high resolution clock using QueryPerformanceCounter
//------------------------------------------------------------

#include <windows.h>
#include <mmsystem.h> // for QueryPerformanceCounter

#include <Clock.h>


Clock::Clock() :
    secondsPerTick_(0)
{
    LARGE_INTEGER ticksPerSecond;

    if(QueryPerformanceFrequency( &ticksPerSecond ) != 0)
    {
        secondsPerTick_ = 1.0 / (double)ticksPerSecond.QuadPart;
    }
}



double Clock::getSeconds() const
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);

    return time.QuadPart * secondsPerTick_;
}

