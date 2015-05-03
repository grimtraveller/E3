
#pragma once

//-----------------------------------------------------------------------------------
// e3_Trace.h
//
// A class that outputs string messages in printf-style
//-----------------------------------------------------------------------------------


namespace e3 { namespace common 
{
    extern void trace(const char* f, ...);
}} // namespace e3::common


// Sends a trace message to all outputs supported by the Tracer::trace
// Use printf-style to format the message.
//
#define TRACE( ... ) e3::common::trace( ##__VA_ARGS__ )


