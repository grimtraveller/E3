

#include <chrono>
#include <thread>

#include <e3_Profiler.h>

using e3::common::Profiler;

/*
//--------------------------------------------------------
// class ProfilerTest
//--------------------------------------------------------
//
class ProfilerTest : public ::testing::Test
{
public:
    ProfilerTest() : sampleRate_(44100) {}

    void sleepMicros(int64_t micros) {
        std::this_thread::sleep_for(std::chrono::microseconds(micros));
    }

protected:
    uint32_t sampleRate_;
    Profiler profiler_;
};



TEST_F(ProfilerTest, ElapsedTime)
{
    int64_t expected = 1000000;

    profiler_.start();
    sleepMicros(expected);
    double elapsed = profiler_.measureElapsedTime();

    EXPECT_GE(elapsed, expected);
    EXPECT_LE(elapsed, expected + 20000);
}


TEST_F(ProfilerTest, CpuPercentage50Percent)
{
    profiler_.start();
    sleepMicros(500000);
    double percent = profiler_.getCpuPercentage(sampleRate_);

    EXPECT_NEAR(50, percent, 2);
}
*/