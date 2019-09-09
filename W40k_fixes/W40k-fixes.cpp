#include <Windows.h>
#include <detours.h>
#include "W40kHacks.h"
#include "HookFunctions.h"
#include <string>


W40kHacks* hacks;
HWND windowInstance;
bool borderless;
int BorderlessPosX;
int BorderlessPosY;
int BorderlessWidth = 0;
int BorderlessHeight = 0;


static BOOL(__stdcall* TrueSetWindowPos)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) = SetWindowPos;
BOOL __stdcall DetourSetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	BOOL result = TrueSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
	if (hWnd == windowInstance)
	{
		RECT rect;
		GetClientRect(hWnd, &rect);	//Inner window - no bars etc.
		auto width = rect.right - rect.left;
		auto height = rect.bottom - rect.top;
		hacks->Update(width, height);
		if (borderless && width > 640 && height > 480)
		{
			bool isAutomaticRes = BorderlessWidth == 0 || BorderlessHeight == 0;
			SetWindowLong(hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			if (isAutomaticRes)
			{
				RECT mainMonitorRes;
				HWND hDesktop = GetDesktopWindow();
				GetWindowRect(hDesktop, &mainMonitorRes);
				if (mainMonitorRes.right - mainMonitorRes.left == width)
				{
					width = mainMonitorRes.right - mainMonitorRes.left;
					height = mainMonitorRes.bottom - mainMonitorRes.top;
				}
			}
			else
			{
				width = BorderlessWidth;
				height = BorderlessHeight;
			}
			result = TrueSetWindowPos(hWnd, hWndInsertAfter, BorderlessPosX, BorderlessPosY, width, height, uFlags);
		}
	}
	else


	return result;
}

static HWND(__stdcall* TrueCreateWindowEx)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = CreateWindowEx;
HWND __stdcall DetourCreateWindowEx(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	auto comp = strstr(lpWindowName, "Fire Warrior");

	auto result = TrueCreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	if (comp != NULL)
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
			CIniReader iniReader("");
			hacks = new W40kHacks();
			borderless = iniReader.ReadBoolean("MAIN", "Borderless", 0);
			BorderlessPosX = iniReader.ReadInteger("MAIN", "BorderlessPositionX", 0);
			BorderlessPosY = iniReader.ReadInteger("MAIN", "BorderlessPositionY", 0);
			BorderlessWidth = iniReader.ReadInteger("MAIN", "BorderlessWidth", 0);
			BorderlessHeight = iniReader.ReadInteger("MAIN", "BorderlessHeight", 0);
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


