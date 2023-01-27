#pragma once
class WriteOffsetMemory
{
public:
	static BOOL Write(_In_opt_ LPCWSTR moduleName, DWORD baseOffset, DWORD arr[], DWORD length, DWORD writeData);
	static HMODULE GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName);
};

