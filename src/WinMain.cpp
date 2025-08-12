#include "Window.h"
#include <stdexcept>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Window wnd{ hInstance, 800, 600, L"Game Engine" };
		while (wnd.ProcessMessages());
	} catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
		return -1;
	}
	return 0;
}