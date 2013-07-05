
//----------------------------------------------------
// Logging.h
// Looging output using
// - boost::log (if enabled with USE_BOOST_LOG)
// - cerr and ::OutputDebugString
//----------------------------------------------------

#pragma once

#include <boost/log/core.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/debug_output_backend.hpp>
#include <boost/log/support/date_time.hpp>


// Verbosity level
enum LogLevel
{
    verbose1,
    verbose2,
    warning,
    error,
};



extern boost::log::sources::severity_logger< LogLevel > globalLogger;

