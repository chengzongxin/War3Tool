#include "pch.h"
#include "WriteOffsetMemory.h"
#include <iostream>
#include <psapi.h>
#include <tlhelp32.h>
using namespace std;

BOOL WriteOffsetMemory::write(DWORD arr[], DWORD size)
{
	DWORD pid;
	HWND hWnd = ::FindWindow(NULL, _T("Warcraft III")); // 获取窗口句柄
	if (NULL == hWnd)
	{
		::MessageBox(NULL, _T("Warcraft III游戏未打开"), _T("错误"), MB_OK);
		return false;
	}
	GetWindowThreadProcessId(hWnd, &pid); // 通过窗口句柄拿到进程ID
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid); // 通过进程ID拿到进程句柄

	HMODULE hModule = GetProcessModuleHandle(pid, _T("Storm.dll"));
	WCHAR path[100] = { 0 };
	GetModuleFileNameEx(hProcess, hModule, path, sizeof(path));//添加 #include "Psapi.h" ;获得某个正在运行的EXE或者DLL的全路径
	MODULEINFO mi;
	GetModuleInformation(hProcess, hModule, &mi, sizeof(MODULEINFO)); // 获取b.dll信息
	// mi.lpBaseOfDll就是b.dll的加载起始地址
	DWORD dwTemp = 0;
	BOOL bRead = false;

	bRead = ReadProcessMemory(hProcess, (LPCVOID)((int)mi.lpBaseOfDll + 0x0002BC58), &dwTemp, sizeof(DWORD), &pid);

	for (size_t i = 0; i < size; i++)
	{
		DWORD addr = arr[i];
		dwTemp += addr;

		if (i != size - 1)
		{
			bRead = ReadProcessMemory(hProcess, (LPCVOID)dwTemp, &dwTemp, sizeof(DWORD), &pid);
			if (NULL == bRead)
			{
				::MessageBox(NULL, _T(""), _T("错误"), MB_OK);
				CloseHandle(hProcess);
				return false;
			}
		}
		else 
		{
			// 最后一个内存地址，作为写入数据
			// 写入数据
			int n_money = 88000000;
			SIZE_T numberOfBytesWritten = 0;
			//36F4B9D0
			BOOL bSuc = WriteProcessMemory(hProcess, (LPVOID)dwTemp, &n_money, sizeof(n_money), &numberOfBytesWritten);
			if (!bSuc)
			{
				::MessageBox(NULL, _T("修改杀敌错误"), _T("错误"), MB_OK);
				CloseHandle(hProcess);
				return false;
			}
		}
	}

	CloseHandle(hProcess);

	return true;
}

HMODULE WriteOffsetMemory::GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName) {	// 根据 PID 、模块名（需要写后缀，如：".dll"），获取模块入口地址。
	MODULEENTRY32 moduleEntry;
	HANDLE handle = NULL;
	handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid); //  获取进程快照中包含在th32ProcessID中指定的进程的所有的模块。
	if (!handle) {
		CloseHandle(handle);
		return NULL;
	}
	ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(handle, &moduleEntry)) {
		CloseHandle(handle);
		return NULL;
	}
	do {
		if (_tcscmp(moduleEntry.szModule, moduleName) == 0) { return (HMODULE)moduleEntry.hModule; }
	} while (Module32Next(handle, &moduleEntry));
	CloseHandle(handle);
	return 0;
}
