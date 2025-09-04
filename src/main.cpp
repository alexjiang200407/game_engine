#include "Game.h"
#include <objbase.h>
#include <stdexcept>

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nCmdShow;

	try
	{
		wchar_t exePath[MAX_PATH];
		GetModuleFileNameW(nullptr, exePath, MAX_PATH);
		logger::Init(exePath);

		auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hr))
		{
			return -1;
		}

		util::Settings::GetSingleton()->ReadAllSettings();

		Game{}.Play();
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
		return -1;
	}
	catch (...)
	{
		MessageBoxA(nullptr, "An unknown error occurred.", "Error", MB_OK | MB_ICONERROR);
		return -1;
	}
	return 0;
}
