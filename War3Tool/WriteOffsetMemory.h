#pragma once
class WriteOffsetMemory
{
public:
	static BOOL write(DWORD arr[], DWORD size);
	static afx_msg HMODULE GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName);
};

