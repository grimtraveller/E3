
//-------------------------------------------------------
// InstrumentChunk.h
//-------------------------------------------------------

#pragma once

#include <vector>

#include <IntegerTypes.h>
#include <EnumHelper.h>

namespace e3 {

    class InstrumentChunk
    {
    public:
        enum LoopMode
        {
            LoopNone = 0,
            LoopForward = 1,
            LoopBackward = 2,
            LoopAlternating = 3
        };

        struct LoopData
        {
            LoopData();

            LoopMode mode_;
            uint32_t start_;
            uint32_t end_;
            uint32_t numRepeats_;
        };
        typedef std::vector<LoopData> LoopVector;

        InstrumentChunk();

        void setGain(int gain)                  { gain_ = gain; }
        void setBaseNote(int baseNote)          { baseNote_ = baseNote; }
        void setKeyLow(int keyLow)              { keyLow_ = keyLow; }
        void setKeyHigh(int keyHigh)            { keyHigh_ = keyHigh; }
        void setVelocityLow(int velocityLow)    { velocityLow_ = velocityLow; }
        void setVelocityHigh(int velocityHigh)  { velocityHigh_ = velocityHigh; }
        void setDetune(int detune)              { detune_ = detune; }

        int getGain() const                     { return gain_; }
        int getBaseNote() const                 { return baseNote_; }
        int getKeyLow() const                   { return keyLow_; }
        int getKeyHigh() const                  { return keyHigh_; }
        int getVelocityLow() const              { return velocityLow_; }
        int getVelocityHigh() const             { return velocityHigh_; }
        int getDetune() const                   { return detune_; }

        void clearLoops()                       { loops_.clear(); }
        void addLoop(const LoopData& data)      { loops_.push_back(data); }
        int getNumLoops() const                 { return loops_.size(); }
        const LoopVector& getLoops() const      { return loops_; }

        typedef EnumNames<LoopMode> LoopModeInfo;
        static LoopModeInfo& getLoopModeInfo()  { return loopModeInfo_s; }

    protected:
        int gain_;
        int baseNote_;
        int keyLow_;
        int keyHigh_;
        int velocityLow_;
        int velocityHigh_;
        int detune_;
        LoopVector loops_;

    protected:
        //! Maps the LoopMode constants to strings. 
        static LoopModeInfo loopModeInfo_s; // TODO: find simpler way
        struct Initializer {
            Initializer();
        };
        static Initializer initializer_s;
    };

}  // namespace e3