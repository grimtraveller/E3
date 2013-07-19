
//----------------------------------------------------
// Logging.h
// Looging output using
// - boost::log (if enabled with USE_BOOST_LOG)
// - cerr and ::OutputDebugString
//----------------------------------------------------

#pragma once


#include <boost/log/core.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/severity_logger.hpp>


// Verbosity level
enum LogLevel
{
    verbose1,
    verbose2,
    warning,
    error,
};



extern boost::log::sources::severity_logger< LogLevel > globalLogger;

