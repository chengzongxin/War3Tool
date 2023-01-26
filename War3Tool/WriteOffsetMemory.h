#pragma once
class WriteOffsetMemory
{
public:
	static BOOL Write(_In_opt_ LPCWSTR moduleName, DWORD baseOffset, DWORD arr[]);
	static afx_msg HMODULE GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName);
};

