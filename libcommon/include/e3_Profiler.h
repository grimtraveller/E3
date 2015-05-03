

//------------------------------------------------------------
// e3_Profiler.h
//
// Measures time in microseconds
//------------------------------------------------------------


#pragma once

#include <chrono>
#include <cstdint>

#include <e3_Trace.h>


using namespace std::chrono;


namespace e3 {
    namespace common {

        class Profiler
        {
        public:
            Profiler(uint32_t sampleRate = 44100)
            {
                init(sampleRate);
            }
            
            void init(uint32_t sampleRate)
            {
                samplePeriod_ = (1 / (double)sampleRate);
                framesTotal_  = 0;
                secondsTotal_ = 0;
                time1_        = time2_ = steady_clock::now();
            }

            void setSampleRate(uint32_t sampleRate)   
            {
                init(sampleRate); 
            }

            void start()
            {
                time1_ = steady_clock::now();
            }


            int64_t measureElapsedTime()
            {
                time2_      = steady_clock::now();
                auto diff   = time2_ - time1_;
                auto micros = duration_cast<microseconds>(diff);

                return micros.count();
            }


            double getCpuPercentage(uint64_t frames)
            {
                double secondsNeeded, percent;

                time2_         = steady_clock::now();
                auto diff      = time2_ - time1_;
                              
                secondsTotal_ += duration_cast<nanoseconds>(diff).count() / 1000000000.0;
                framesTotal_  += frames;

                secondsNeeded  = framesTotal_ * samplePeriod_;
                percent        = (secondsTotal_ / secondsNeeded) * 100;

                return percent;
            }


            // Rounds the given percent value and returns true if the rounded value is different from 
            // the last rounded value.
            //
            bool smooth(double& percent)
            {
                if (percent == 0) return false;

                percent = round(percent * 100) / 100.0;
                if (percent != lastPercent_) {
                    lastPercent_ = percent;
                    return true;
                }
                return false;
            }

        private:
            double samplePeriod_   = 0;
            double secondsTotal_   = 0;
            double lastPercent_    = -1;
            uint64_t framesTotal_  = 0;

            typedef steady_clock::time_point TimePoint;
            TimePoint time1_, time2_;
        };
    }
} // namespace e3::common