#include "Game.h"
#include "window/Window.h"
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
		logger::Init();
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
