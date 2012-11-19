
#pragma once


class Trace
{
public:
	static void trace(const char* f, ...);
};


// Sends a trace message to an output.
// The message can be formatted in printf-style.
//
#define TRACE( ... ) Trace::trace( ##__VA_ARGS__ )



