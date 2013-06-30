
//---------------------------------------------------
// InstrumentChunk.cpp
//---------------------------------------------------

#include <InstrumentChunk.h>


InstrumentChunk::LoopModeInfo InstrumentChunk::loopModeInfo_s;

InstrumentChunk::Initializer InstrumentChunk::initializer_s;


InstrumentChunk::Initializer::Initializer()
{
    loopModeInfo_s.add(LoopNone,        "NONE");
    loopModeInfo_s.add(LoopForward,     "FORWARD");
    loopModeInfo_s.add(LoopBackward,    "BACKWARD");
    loopModeInfo_s.add(LoopAlternating, "ALTERNATING");
}



InstrumentChunk::InstrumentChunk() :
    gain_(1),
    baseNote_(60),
    keyLow_(0),
    keyHigh_(127),
    velocityLow_(0),
    velocityHigh_(127),
    detune_(0)
{}



InstrumentChunk::LoopData::LoopData() :
    mode_(LoopNone),
    start_(0),
    end_(0),
    numRepeats_(0)
{}


/*
InstrumentChunk::LoopModeInfo AudioFile::InstrumentChunk::loopModeInfo_s = boost::assign::map_list_of
    (LoopNone,         "None")
    (LoopForward,      "Forward")
    (LoopBackward,     "Backward")
    (LoopAlternating,  "Alternating");
*/


