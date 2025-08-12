#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MessageBoxA(NULL, "Hello from WinMain!", "Game Engine", MB_OK);
	return 0;
}