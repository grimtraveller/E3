
//------------------------------------------------------------
// Clock.h
//
// A high resolution clock using QueryPerformanceCounter
//------------------------------------------------------------

#pragma once


class Clock
{
public:
    Clock();
    double getSeconds() const;

private:
    double secondsPerTick_;
};



