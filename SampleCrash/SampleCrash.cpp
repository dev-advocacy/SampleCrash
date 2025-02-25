// SampleCrash.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include "LocalDumpManager.h"






int main(int argc, char* argv[])
{
	LocalDumpManager localdump;
	HRESULT hr = S_OK;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("install", "install the localdump folder")
		("uninstall", "unstall the localdump folder")
		("info", "unstall the localdump folder")
		("monitor", "monitor the localdump folder")
		("crash", "crash the application");

	po::variables_map vm;

	try 
	{
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (const po::unknown_option& e) {
		std::cerr << "Unknown option: " << e.get_option_name() << std::endl;
		std::cout << desc << "\n";
		return 1;
	}

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 0;
	}
	if (vm.count("crash"))
	{
		int* p = nullptr;
		*p = 3;
		int s;
		while (1) {
			s = *p;
			p++;
		}
	}
	if (vm.count("install"))
	{
		auto executable = localdump.GetExecutableName();
		auto dumppath = localdump.GetLocalDumpPath();
		hr = localdump.WriteLocaldumpInformation(executable, dumppath);
		if (FAILED(hr))
		{
			std::cout << "Failed to install localdump folder." << std::endl;
			return -1;
		}
	}
	if (vm.count("uninstall"))
	{
		hr = localdump.RemoveLocaldumpInformation();
		if (FAILED(hr))
		{
			std::cout << "Failed to uninstall localdump folder." << std::endl;
			return -1;
		}
	}
	if (vm.count("info"))
	{
		hr = localdump.GetLocalDumpInformation(HKEY_LOCAL_MACHINE);
		if (FAILED(hr))
		{
			std::cout << "Failed to get localdump information." << std::endl;
			return -1;
		}
		hr = localdump.GetLocalDumpInformation(HKEY_CURRENT_USER);
		if (FAILED(hr))
		{
			std::cout << "Failed to get localdump information." << std::endl;
			return -1;
		}
		
	}
	if (vm.count("monitor"))
	{
		localdump.MonitorRegistryKey();
	}




}

