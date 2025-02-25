// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// command line parser
#include <boost/program_options.hpp>
#include <iostream>
#include <Windows.h>
#include <Shlobj.h>
#include <filesystem>
#include <atlbase.h>



namespace	po = boost::program_options;
namespace fs = std::filesystem;
#endif //PCH_H
