

#include <windows.h>
#include <mmsystem.h> // for QueryPerformanceCounter

#include <core/CPUProfiler.h>



//---------------------------------------------------
// class Clock
//---------------------------------------------------

Clock::Clock() :
    usePerformanceCounter_(false),
    secondsPerTick_(0)
{
    LARGE_INTEGER ticksPerSecond;

    if(QueryPerformanceFrequency( &ticksPerSecond ) != 0)
    {
        usePerformanceCounter_ = true;
        secondsPerTick_ = 1.0 / (double)ticksPerSecond.QuadPart;
    }
}



double Clock::getTime() const
{
    if(usePerformanceCounter_)
    {
        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);
        return time.QuadPart * secondsPerTick_;
    }
    else {
        return timeGetTime() * .001;
    }
}


CPUProfiler::CPUProfiler() : 
    samplePeriod_(0),
    averageLoad_(0),
    startTime_(0)
{}



void CPUProfiler::begin()
{
    startTime_ = clock_.getTime();
}



void CPUProfiler::end(int framesProcessed)
{
    if( framesProcessed > 0 )
    {
        double endTime              = clock_.getTime();
        double secondsFor100Percent = framesProcessed * samplePeriod_;
        double measuredLoad         = (endTime - startTime_) / secondsFor100Percent;

        // Low pass filter the calculated CPU load to reduce jitter using a simple IIR low pass filter. 
#define LOWPASS_COEFFICIENT_0   (0.9)
#define LOWPASS_COEFFICIENT_1   (0.99999 - LOWPASS_COEFFICIENT_0)

        averageLoad_ = (LOWPASS_COEFFICIENT_0 * averageLoad_) + (LOWPASS_COEFFICIENT_1 * measuredLoad);
    }
}
