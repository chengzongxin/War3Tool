#include "pch.h"
#include "WriteOffsetMemory.h"
#include <iostream>
#include <psapi.h>
#include <tlhelp32.h>
using namespace std;

BOOL WriteOffsetMemory::write(DWORD arr[], DWORD size)
{
	DWORD pid;
	HWND hWnd = ::FindWindow(NULL, _T("Warcraft III")); // ��ȡ���ھ��
	if (NULL == hWnd)
	{
		::MessageBox(NULL, _T("Warcraft III��Ϸδ��"), _T("����"), MB_OK);
		return false;
	}
	GetWindowThreadProcessId(hWnd, &pid); // ͨ�����ھ���õ�����ID
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid); // ͨ������ID�õ����̾��

	HMODULE hModule = GetProcessModuleHandle(pid, _T("Storm.dll"));
	WCHAR path[100] = { 0 };
	GetModuleFileNameEx(hProcess, hModule, path, sizeof(path));//��� #include "Psapi.h" ;���ĳ���������е�EXE����DLL��ȫ·��
	MODULEINFO mi;
	GetModuleInformation(hProcess, hModule, &mi, sizeof(MODULEINFO)); // ��ȡb.dll��Ϣ
	// mi.lpBaseOfDll����b.dll�ļ�����ʼ��ַ
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
				::MessageBox(NULL, _T(""), _T("����"), MB_OK);
				CloseHandle(hProcess);
				return false;
			}
		}
		else 
		{
			// ���һ���ڴ��ַ����Ϊд������
			// д������
			int n_money = 88000000;
			SIZE_T numberOfBytesWritten = 0;
			//36F4B9D0
			BOOL bSuc = WriteProcessMemory(hProcess, (LPVOID)dwTemp, &n_money, sizeof(n_money), &numberOfBytesWritten);
			if (!bSuc)
			{
				::MessageBox(NULL, _T("�޸�ɱ�д���"), _T("����"), MB_OK);
				CloseHandle(hProcess);
				return false;
			}
		}
	}

	CloseHandle(hProcess);

	return true;
}

HMODULE WriteOffsetMemory::GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName) {	// ���� PID ��ģ��������Ҫд��׺���磺".dll"������ȡģ����ڵ�ַ��
	MODULEENTRY32 moduleEntry;
	HANDLE handle = NULL;
	handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid); //  ��ȡ���̿����а�����th32ProcessID��ָ���Ľ��̵����е�ģ�顣
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
