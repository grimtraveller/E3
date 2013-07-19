

//------------------------------------------------------------
// CPUProfiler.h
//
// Measures CPU performance for audio applications
//------------------------------------------------------------


#pragma once


class CPUProfiler
{
public:
    CPUProfiler();

    void begin();
    void end(int framesProcessed);
    void reset()                        { averageLoad_ = 0; }

    void setSampleRate(int sampleRate)  { samplePeriod_ = 1 / (double)sampleRate; }
    double getAverageLoad() const       { return averageLoad_; }

private:
    double samplePeriod_;
    double averageLoad_;
    double startTime_;

    Clock clock_;
};