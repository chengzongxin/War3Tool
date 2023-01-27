#include "pch.h"
#include "WriteOffsetMemory.h"
#include <iostream>
#include <psapi.h>
#include <tlhelp32.h>
using namespace std;

HWND WriteOffsetMemory::GetWindow()
{
	HWND hWnd = ::FindWindow(NULL, _T("Warcraft III")); // ��ȡ���ھ��
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
		::MessageBox(NULL, _T("Warcraft III��Ϸδ��"), _T("����"), MB_OK);
		return false;
	}
	DWORD pid;
	GetWindowThreadProcessId(hWnd, &pid); // ͨ�����ھ���õ�����ID
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid); // ͨ������ID�õ����̾��
	HMODULE hModule = GetProcessModuleHandle(pid, moduleName);
	WCHAR path[100] = { 0 };
	GetModuleFileNameEx(hProcess, hModule, path, sizeof(path));//��� #include "Psapi.h" ;���ĳ���������е�EXE����DLL��ȫ·��
	MODULEINFO mi;
	GetModuleInformation(hProcess, hModule, &mi, sizeof(MODULEINFO)); // ��ȡb.dll��Ϣ
	// mi.lpBaseOfDll����b.dll�ļ�����ʼ��ַ
	DWORD dwTemp = 0;
	BOOL bRead = false;

	bRead = ReadProcessMemory(hProcess, (LPCVOID)((int)mi.lpBaseOfDll + baseOffset), &dwTemp, sizeof(DWORD), &pid);
	//CString str;
	//str.Format(_T("��ַ��%x + ƫ�ƣ�%x,  ��ȡ�ڴ棺%x"),mi.lpBaseOfDll,baseOffset, dwTemp);
	//::MessageBox(NULL, str, _T("����"), MB_OK);
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
				str.Format(_T("��ȡ�ڴ����%d��ƫ�ƣ�%d"), dwTemp,i);
				::MessageBox(NULL, str, _T("����"), MB_OK);
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
				str.Format(_T("д���ڴ����%d"), dwTemp);
				::MessageBox(NULL, str, _T("����"), MB_OK);
				CloseHandle(hProcess);
				return false;
			}
			
 			if (force == false && dwCurrent >= writeData)
			{
				// ��ֵ�ܴ󣬲����޸�
				CloseHandle(hProcess);
				return false;
			}

			// ���һ���ڴ��ַ����Ϊд������
			// д������
			int n_Data = writeData;
			SIZE_T numberOfBytesWritten = 0;
			BOOL bSuc = WriteProcessMemory(hProcess, (LPVOID)dwTemp, &n_Data, sizeof(n_Data), &numberOfBytesWritten);
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

BOOL WriteOffsetMemory::UpPrivilege()
{
	HANDLE token_handle;
	//�򿪷�������
	if (!OpenProcessToken(GetCurrentProcess(),       //Ҫ�޸�Ȩ�޵Ľ��̾��
		TOKEN_ALL_ACCESS,          //Ҫ�����ƽ��к��ֲ���
		&token_handle              //��������
	))
	{
		printf("openProcessToken error");
	}

	LUID luid;
	if (!LookupPrivilegeValue(NULL,                 //�鿴��ϵͳ������ΪNULL
		SE_DEBUG_NAME,        //Ҫ�鿴����Ȩ����
		&luid                 //�������ձ�ʶ��
	))
	{
		printf("lookupPrivilegevalue error");
	}

	TOKEN_PRIVILEGES tkp;
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//������������Ȩ��
	if (!AdjustTokenPrivileges(token_handle,    //���ƾ��
		FALSE,           //�Ƿ����Ȩ��
		&tkp,            //�µ���Ȩ��Ȩ����Ϣ
		sizeof(tkp),     //��Ȩ��Ϣ��С
		NULL,            //����������Ȩ��Ϣ��ǰ״̬��buffer
		NULL             //��������С
	))
	{
		printf("adjust error");
	}

	printf("sucessful");
	return 1;
}