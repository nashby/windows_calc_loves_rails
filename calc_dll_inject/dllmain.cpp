#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include "detours/detours.h"

#pragma comment(lib, "detours/detours.lib")
#pragma comment(lib, "detours/detoured.lib")

long INPUT_PARENT_HEIGHT = 50L;
long INPUT_PARENT_WIDTH = 385L;

long INPUT_HEIGHT = 28L;
long INPUT_WIDTH = 362L;

LPCWSTR ROR_BUTTON = L"RoR";
LPCWSTR RUBY_LOVE = L"<3 <3 <3 Ruby <3 <3 <3    ";

VOID WINAPI NullExport()
{
    
}

BOOL CALLBACK FindInput(HWND hwnd, LPARAM lParam)
{
	RECT lprect;
	LPWSTR pszMem;
	int cTxtLen;
	cTxtLen = GetWindowTextLength(hwnd);
	pszMem = (LPWSTR) VirtualAlloc((LPVOID) NULL, 
                    (DWORD) (cTxtLen + 1), MEM_COMMIT, 
                    PAGE_READWRITE);
	GetWindowText(hwnd, pszMem, 10);
	GetWindowRect(hwnd, &lprect);
	if ((lprect.bottom - lprect.top) == INPUT_HEIGHT && (lprect.right - lprect.left) == INPUT_WIDTH)
		SetWindowText(hwnd, RUBY_LOVE);
	VirtualFree(pszMem, 0, MEM_RELEASE); 
	return true;
}

BOOL CALLBACK EnumChildWndProc(HWND hwnd, LPARAM lParam)
{
	RECT lprect;
	LPWSTR pszMem;
	int cTxtLen;
	cTxtLen = GetWindowTextLength(hwnd);
	pszMem = (LPWSTR) VirtualAlloc((LPVOID) NULL, 
                    (DWORD) (cTxtLen + 1), MEM_COMMIT, 
                    PAGE_READWRITE);
	GetWindowText(hwnd, pszMem, 10);
	GetWindowRect(hwnd, &lprect);
	if ((lprect.bottom - lprect.top) == INPUT_PARENT_HEIGHT && (lprect.right - lprect.left) == INPUT_PARENT_WIDTH)
		EnumChildWindows(GetParent(hwnd), FindInput, 0); 
	VirtualFree(pszMem, 0, MEM_RELEASE); 
	return true;
}
extern "C" BOOL (WINAPI * Real_GetMessage)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax) = GetMessage;
BOOL WINAPI Mine_GetMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
	LPWSTR pszMem;
	int cTxtLen;
	cTxtLen = GetWindowTextLength(lpMsg->hwnd);
	pszMem = (LPWSTR) VirtualAlloc((LPVOID) NULL, 
                    (DWORD) (cTxtLen + 1), MEM_COMMIT, 
                    PAGE_READWRITE);
	GetWindowText(lpMsg->hwnd, pszMem, 4);
	
	if(lpMsg->message == 513 && wcscmp(pszMem, ROR_BUTTON) == 0)
	{
		EnumChildWindows(GetParent(GetParent(lpMsg->hwnd)), EnumChildWndProc, 0); 
		system("start http://rubyonrails.org");
	}
	VirtualFree(pszMem, 0, MEM_RELEASE); 
	return Real_GetMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
}
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)Real_GetMessage, Mine_GetMessage);
		DetourTransactionCommit();
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)Real_GetMessage, Mine_GetMessage);
        DetourTransactionCommit();
    }
    return TRUE;
}
