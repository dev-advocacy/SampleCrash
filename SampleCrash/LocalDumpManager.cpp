#include "pch.h"
#include <fstream>
#include <conio.h>
#include "LocalDumpManager.h"

// define as const  L"SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps"
#define REGKEYLOCALDUMP L"SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps"
#define EXCLUDEDAPPLICATIONS L"SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\ExcludedApplications"
#define DUMPCOUNT 10
#define DUMPTYPE  0
#define DUMPFLAGS 4368

BOOL LocalDumpManager::IsElevated()
{
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return fRet;
}
std::wstring LocalDumpManager::GetExecutableName()
{
	TCHAR szFileName[MAX_PATH];
	DWORD result = GetModuleFileName(NULL, szFileName, MAX_PATH);
	if (result == 0) {
		return L"";
	}
	std::wstring fullPath(szFileName);
	size_t pos = fullPath.find_last_of(L"\\/");
	if (pos != std::wstring::npos) {
		return fullPath.substr(pos + 1);
	}
	return fullPath;
}

HRESULT LocalDumpManager::GetLocalDumpInformation(HKEY hKey)
{
	CRegKey key;
	auto executable = GetExecutableName();
	std::wstring keylocaldump = REGKEYLOCALDUMP;
	keylocaldump.append(L"\\");
	keylocaldump.append(executable);

	// dump all information from the registry
	if (key.Open(hKey, keylocaldump.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)
	{
		std::wcout << (hKey == HKEY_LOCAL_MACHINE ? _T("HKLM\\") : _T("HKCU\\"));
		std::wcout << keylocaldump << _T("\texist") << std::endl;
		DWORD dumpcount = 0;
		DWORD dumptype = 0;
		DWORD dumpflags = 0;

		std::wstring dumpfolder;
		WCHAR szdumpfolder[MAX_PATH];
		ULONG szdumpfolderlen = MAX_PATH;

		if (key.QueryDWORDValue(L"DumpCount", dumpcount) != ERROR_SUCCESS)
		{
			dumpcount = -1;
		}
		if (key.QueryDWORDValue(L"DumpType", dumptype) != ERROR_SUCCESS)
		{
			dumptype = -1;
		}
		if (key.QueryDWORDValue(L"CustomDumpFlags", dumpflags) != ERROR_SUCCESS)
		{
			dumpflags = -1;
		}
		if (key.QueryStringValue(L"DumpFolder", szdumpfolder, &szdumpfolderlen) != ERROR_SUCCESS)
		{
			dumpfolder = L"";
		}
		else
		{
			dumpfolder = szdumpfolder;
		}
		dumpfolder = szdumpfolder;
		// print all information

		std::wcout << _T("DumpCount:\t\t") << (dumpcount == -1 ? L"N/A" : std::to_wstring(dumpcount)) << std::endl;
		std::wcout << _T("DumpType:\t\t") << (dumptype == -1 ? L"N/A" : std::to_wstring(dumptype)) << std::endl;
		std::wcout << _T("CustomDumpFlags:\t") << (dumpflags == -1 ? L"N/A" : std::to_wstring(dumpflags)) << std::endl;
		std::wcout << _T("DumpFolder:\t\t") << dumpfolder << std::endl;

		// test if the executable exist on ExcludedApplications
		CRegKey key1;

		std::wstring excludedapplications = EXCLUDEDAPPLICATIONS;
		excludedapplications.append(L"\\");
		excludedapplications.append(executable);

		if (key1.Open(hKey, excludedapplications.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)
		{
			DWORD dexcludeapplication = 0;
			if (key1.QueryDWORDValue(executable.c_str(), dexcludeapplication))
			{
				std::wcout << _T("the executable: ") << executable << _T(" is excluded") << std::endl;
			}
			else
			{
				std::wcout << _T("the executable: ") << executable << _T(" is not excluded") << std::endl;
			}
		}
		else
		{
			std::wcout << (hKey == HKEY_LOCAL_MACHINE ? _T("HKLM\\") : _T("HKCU\\"));
			std::wcout << _T("SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\ExcludedApplications\tdo not exist") << std::endl;
		}
	}
	else
	{

		std::wcout << _T("the key: ");
		std::wcout << (hKey == HKEY_LOCAL_MACHINE ? _T("HKLM\\") : _T("HKCU\\"));
		std::wcout << keylocaldump << _T(" do not exist") << std::endl;
	}
	return S_OK;
}

HRESULT LocalDumpManager::RemoveLocaldumpInformation()
{
	std::cout << "Uninstalling localdump folder..." << std::endl;
	auto executable = GetExecutableName();
	std::wstring keylocaldump = REGKEYLOCALDUMP;

	CRegKey key;
	HKEY hKey;
	hKey = IsElevated() ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;

	// Check if the key exists before attempting to delete it
	CRegKey subKey;
	if (subKey.Open(hKey, (keylocaldump + L"\\" + executable).c_str(), KEY_READ) != ERROR_SUCCESS)
	{
		std::wcout << (hKey == HKEY_LOCAL_MACHINE ? _T("HKLM\\") : _T("HKCU\\"));
		std::wcout << (keylocaldump + L"\\" + executable).c_str() << _T(" do not exist") << std::endl;
		return E_FAIL;
	}
	if (key.Open(hKey, keylocaldump.c_str(), KEY_SET_VALUE) != ERROR_SUCCESS)
	{
		return E_FAIL;
	}
	if (key.RecurseDeleteKey(executable.c_str()) != ERROR_SUCCESS)
	{
		std::cout << "Failed to delete registry key." << std::endl;
		return E_FAIL;
	}
	std::wcout << (hKey == HKEY_LOCAL_MACHINE ? _T("HKLM\\") : _T("HKCU\\"));
	std::wcout << keylocaldump << std::endl;
	return S_OK;
}

HRESULT LocalDumpManager::WriteLocaldumpInformation(std::wstring executable, std::wstring dumppath)
{
	CRegKey key;
	HKEY hKey;

	std::wstring keylocaldump = REGKEYLOCALDUMP;
	hKey = IsElevated() ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
	// test if SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps
	if (key.Open(hKey, keylocaldump.c_str(), KEY_SET_VALUE) != ERROR_SUCCESS)
	{
		// we create SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps
		if (key.Create(hKey, keylocaldump.c_str()) != ERROR_SUCCESS)
		{
			std::cout << "Failed to create registry key." << std::endl;
			return E_FAIL;
		}
	}
	keylocaldump.append(L"\\");
	keylocaldump.append(executable);
	// test if SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\<executable>
	if (key.Open(hKey, keylocaldump.c_str(), KEY_SET_VALUE) != ERROR_SUCCESS)
	{
		if (key.Create(hKey, keylocaldump.c_str()) != ERROR_SUCCESS)
		{
			std::cout << "Failed to create registry key." << std::endl;
			return E_FAIL;
		}
	}
	// set DumpCount
	if (key.SetDWORDValue(L"DumpCount", DUMPCOUNT) != ERROR_SUCCESS)
	{
		std::cout << "Failed to set registry value. DumpCount" << std::endl;
		return E_FAIL;
	}
	// set DumpType
	if (key.SetDWORDValue(L"DumpType", DUMPTYPE) != ERROR_SUCCESS)
	{
		std::cout << "Failed to set registry value. DumpType" << std::endl;
		return E_FAIL;
	}
	// set DumpFlags
	if (key.SetDWORDValue(L"CustomDumpFlags", DUMPFLAGS) != ERROR_SUCCESS)
	{
		std::cout << "Failed to set registry value. DumpFlags" << std::endl;
		return E_FAIL;
	}

	// set DumpFolder to <dumppath> using REG_EXPAND_SZ
	if (key.SetStringValue(L"DumpFolder", dumppath.c_str(), REG_EXPAND_SZ) != ERROR_SUCCESS)
	{
		std::cout << "Failed to set registry value. DumpFolder" << std::endl;
		return E_FAIL;
	}
	std::wcout << _T("localdump folder installed") << std::endl;
	std::wcout << (hKey == HKEY_LOCAL_MACHINE ? _T("HKLM\\") : _T("HKCU\\"));
	std::wcout << keylocaldump << std::endl;
	std::wcout << _T("DumpCount:\t\t") << DUMPCOUNT << std::endl;
	std::wcout << _T("DumpType:\t\t") << DUMPTYPE << std::endl;
	std::wcout << _T("CustomDumpFlags:\t") << DUMPFLAGS << std::endl;
	std::wcout << _T("DumpFolder:\t\t") << dumppath << std::endl;


	return S_OK;
}
std::wstring LocalDumpManager::GetLocalDumpPath()
{
	std::wstring r;
	WCHAR* szSystemPath = nullptr;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, nullptr, &szSystemPath)))
	{
		fs::path tempPath(szSystemPath);
		tempPath /= L"temp";
		tempPath /= L"localdump";

		if (!fs::exists(tempPath))
		{
			fs::create_directories(tempPath);
		}
		return(tempPath.wstring());
	}
	CoTaskMemFree(szSystemPath);
	return std::wstring();
}

void LocalDumpManager::MonitorRegistryKey()
{
	HKEY hKey;
	HKEY hSubKey;
	std::wstring subKey = REGKEYLOCALDUMP;	

	hKey = IsElevated() ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;

	std::wstring keylocaldump = REGKEYLOCALDUMP;
	std::wstring executable = GetExecutableName();

	std::wcout << _T("Monitoring registry key: ");
	std::wcout << (hKey == HKEY_LOCAL_MACHINE ? _T("HKLM\\") : _T("HKCU\\"));
	std::wcout << subKey << _T(" Press CTRL+C to exit") << std::endl;
	while (true)
	{

		if (RegOpenKeyEx(hKey, (keylocaldump + L"\\" + executable).c_str(), 0, KEY_NOTIFY | KEY_READ, &hSubKey) == ERROR_SUCCESS)
		{
				if (RegNotifyChangeKeyValue(hSubKey, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET, NULL, FALSE) == ERROR_SUCCESS)
				{
					std::wcout << L"Registry key changed: " << subKey << std::endl;			
					auto dumppath = GetLocalDumpPath();
					dumppath.append(_T("\\"));
					dumppath.append(_T("registry.log"));
				
					// append to a registry.log a new entry with date time
					std::wofstream logFile(dumppath, std::ios::app);
					if (logFile.is_open())
					{
						auto t = std::time(nullptr);
						auto tm = std::tm();
						localtime_s(&tm, &t);
						logFile << std::put_time(&tm, L"%Y-%m-%d %H:%M:%S") << L" - Registry key changed: " << subKey << L"\n";
						logFile.close();
					}
					RegCloseKey(hSubKey);
				}				
				RegCloseKey(hKey);
		}
		else
		{
			std::wcout << L"Failed to open registry key: " << subKey << std::endl;
		}		
	}
}
