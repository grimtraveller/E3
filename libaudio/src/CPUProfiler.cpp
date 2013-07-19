

#include <Clock.h>
#include <CPUProfiler.h>




CPUProfiler::CPUProfiler() : 
    samplePeriod_(0),
    averageLoad_(0),
    startTime_(0)
{}



void CPUProfiler::begin()
{
    startTime_ = clock_.getSeconds();
}



void CPUProfiler::end(int framesProcessed)
{
    if( framesProcessed > 0 )
    {
        double endTime              = clock_.getSeconds();
        double secondsFor100Percent = framesProcessed * samplePeriod_;
        double measuredLoad         = (endTime - startTime_) / secondsFor100Percent;

        // Low pass filter the calculated CPU load to reduce jitter using a simple IIR low pass filter. 
#define LOWPASS_COEFFICIENT_0   (0.9)
#define LOWPASS_COEFFICIENT_1   (0.99999 - LOWPASS_COEFFICIENT_0)

        averageLoad_ = (LOWPASS_COEFFICIENT_0 * averageLoad_) + (LOWPASS_COEFFICIENT_1 * measuredLoad);
    }
}
