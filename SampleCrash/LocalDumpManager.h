#pragma once
class LocalDumpManager
{
public:
	static std::wstring GetExecutableName();
	static std::wstring GetLocalDumpPath();
	HRESULT WriteLocaldumpInformation(std::wstring executable, std::wstring dumppath);
	HRESULT RemoveLocaldumpInformation();
	HRESULT GetLocalDumpInformation(HKEY hKey);
	void MonitorRegistryKey();
private:
	static BOOL IsElevated();
};

