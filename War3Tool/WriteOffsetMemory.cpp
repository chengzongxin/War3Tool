#include "pch.h"
#include "WriteOffsetMemory.h"
#include <iostream>
#include <psapi.h>
#include <tlhelp32.h>
using namespace std;

HWND WriteOffsetMemory::GetWindow()
{
	HWND hWnd = ::FindWindow(NULL, _T("Warcraft III")); // 获取窗口句柄
	return hWnd;
}

// "Game.dll"+0x00BE40A4
// 4
// 14
// 3c
// 78
BOOL WriteOffsetMemory::Write(_In_opt_ LPCWSTR moduleName, DWORD baseOffset, DWORD arr[], DWORD length, DWORD writeData, BOOL force)
{
	HWND hWnd = GetWindow();
	if (NULL == hWnd)
	{
		::MessageBox(NULL, _T("Warcraft III游戏未打开"), _T("错误"), MB_OK);
		return false;
	}
	DWORD pid;
	GetWindowThreadProcessId(hWnd, &pid); // 通过窗口句柄拿到进程ID
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid); // 通过进程ID拿到进程句柄
	HMODULE hModule = GetProcessModuleHandle(pid, moduleName);
	WCHAR path[100] = { 0 };
	GetModuleFileNameEx(hProcess, hModule, path, sizeof(path));//添加 #include "Psapi.h" ;获得某个正在运行的EXE或者DLL的全路径
	MODULEINFO mi;
	GetModuleInformation(hProcess, hModule, &mi, sizeof(MODULEINFO)); // 获取b.dll信息
	// mi.lpBaseOfDll就是b.dll的加载起始地址
	DWORD dwTemp = 0;
	BOOL bRead = false;

	bRead = ReadProcessMemory(hProcess, (LPCVOID)((int)mi.lpBaseOfDll + baseOffset), &dwTemp, sizeof(DWORD), &pid);
	//CString str;
	//str.Format(_T("基址：%x + 偏移：%x,  读取内存：%x"),mi.lpBaseOfDll,baseOffset, dwTemp);
	//::MessageBox(NULL, str, _T("错误"), MB_OK);
	for (size_t i = 0; i < length; i++)
	{
		DWORD addr = arr[i];
		dwTemp += addr;

		if (i != length - 1)
		{
			bRead = ReadProcessMemory(hProcess, (LPCVOID)dwTemp, &dwTemp, sizeof(DWORD), &pid);
			if (NULL == bRead)
			{
				CString str;
				str.Format(_T("读取内存错误：%d，偏移：%d"), dwTemp,i);
				::MessageBox(NULL, str, _T("错误"), MB_OK);
				CloseHandle(hProcess);
				return false;
			}
		}
		else 
		{
			DWORD dwCurrent = 0;
			bRead = ReadProcessMemory(hProcess, (LPCVOID)dwTemp, &dwCurrent, sizeof(DWORD), &pid);
			if (NULL == bRead)
			{
				CString str;
				str.Format(_T("写入内存错误：%d"), dwTemp);
				::MessageBox(NULL, str, _T("错误"), MB_OK);
				CloseHandle(hProcess);
				return false;
			}
			
 			if (force == false && dwCurrent >= writeData)
			{
				// 数值很大，不用修改
				CloseHandle(hProcess);
				return false;
			}

			// 最后一个内存地址，作为写入数据
			// 写入数据
			int n_Data = writeData;
			SIZE_T numberOfBytesWritten = 0;
			BOOL bSuc = WriteProcessMemory(hProcess, (LPVOID)dwTemp, &n_Data, sizeof(n_Data), &numberOfBytesWritten);
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

BOOL WriteOffsetMemory::UpPrivilege()
{
	HANDLE token_handle;
	//打开访问令牌
	if (!OpenProcessToken(GetCurrentProcess(),       //要修改权限的进程句柄
		TOKEN_ALL_ACCESS,          //要对令牌进行何种操作
		&token_handle              //访问令牌
	))
	{
		printf("openProcessToken error");
	}

	LUID luid;
	if (!LookupPrivilegeValue(NULL,                 //查看的系统，本地为NULL
		SE_DEBUG_NAME,        //要查看的特权名称
		&luid                 //用来接收标识符
	))
	{
		printf("lookupPrivilegevalue error");
	}

	TOKEN_PRIVILEGES tkp;
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//调整访问令牌权限
	if (!AdjustTokenPrivileges(token_handle,    //令牌句柄
		FALSE,           //是否禁用权限
		&tkp,            //新的特权的权限信息
		sizeof(tkp),     //特权信息大小
		NULL,            //用来接收特权信息当前状态的buffer
		NULL             //缓冲区大小
	))
	{
		printf("adjust error");
	}

	printf("sucessful");
	return 1;
}