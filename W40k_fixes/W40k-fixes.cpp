#include <Windows.h>
#include <detours.h>
#include "W40kHacks.h"
#include "HookFunctions.h"
#include <string>


W40kHacks* hacks;
HWND windowInstance;

static BOOL(__stdcall* TrueSetWindowPos)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) = SetWindowPos;
BOOL __stdcall DetourSetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	auto result = TrueSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
	if (hWnd == windowInstance)
	{
		RECT* rect = new RECT();
		GetClientRect(hWnd, rect);	//Inner window - no bars etc.
		hacks->Update(rect->right - rect->left, rect->bottom - rect->top);
		delete rect;
	}

	return result;
}

static HWND(__stdcall* TrueCreateWindowEx)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = CreateWindowEx;
HWND __stdcall DetourCreateWindowEx(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	auto result = TrueCreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	auto comp = strstr(lpWindowName, "Fire Warrior");
	if (comp > NULL)
		windowInstance = result;	//Store handle of game's window

	return result;
}


BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		HMODULE baseModule = GetModuleHandle(NULL);
		char baseModuleName[MAX_PATH];
		GetModuleFileName(baseModule, baseModuleName, sizeof(baseModuleName));
		int indexOfLastPathNode = StrEndsWith(baseModuleName, sizeof(baseModuleName), '\\') + 1;
		char exeName[MAX_PATH];
		strcpy_s(exeName, baseModuleName + indexOfLastPathNode);
		StrToLower(exeName, sizeof(exeName));
		
		if (std::strstr((const char*)&exeName, "firewarrior.exe"))
		{
			hacks = new W40kHacks();
		}

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)TrueSetWindowPos, DetourSetWindowPos);
		DetourAttach(&(PVOID&)TrueCreateWindowEx, DetourCreateWindowEx);
		DetourTransactionCommit();

	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)TrueSetWindowPos, DetourSetWindowPos);
		DetourDetach(&(PVOID&)TrueCreateWindowEx, DetourCreateWindowEx);
		DetourTransactionCommit();
	}
	return TRUE;
}


