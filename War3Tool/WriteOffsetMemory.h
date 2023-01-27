#pragma once
class WriteOffsetMemory
{
public:
	static HWND GetWindow();
	static BOOL Write(_In_opt_ LPCWSTR moduleName, DWORD baseOffset, DWORD arr[], DWORD length, DWORD writeData, BOOL force = false);
	static HMODULE GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName);
	static BOOL UpPrivilege();
};

