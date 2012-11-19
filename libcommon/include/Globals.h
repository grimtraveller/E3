
#pragma once

//-------------------------------------------------
// Includes
//-------------------------------------------------

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

#ifdef max
#undef max
#endif

#include <boost/filesystem.hpp>
typedef boost::filesystem::path Path;

#include <boost/property_tree/ptree.hpp>
typedef boost::property_tree::ptree XmlTree;

