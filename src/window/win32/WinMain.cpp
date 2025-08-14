#include "window/win32/Window.h"
#include <stdexcept>

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nCmdShow;

	try
	{
		logger::Init();
		Window wnd{ hInstance, 800, 600, L"Game Engine" };
		while (wnd.ProcessMessages())
		{
			while (const auto key = wnd.kbd.ReadKey())
			{
				if (key->code == 65)
				{
					logger::info("A was pressed");
				}
			}

			while (const auto mouseEvt = wnd.mouse.ReadEvent())
			{
				if (mouseEvt->state.all(Mouse::StateFlags::kInsideWindow) &&
				    mouseEvt->type == Mouse::Event::Type::kMove)
				{
					logger::info("Moving Inside Window");
				}
			}
		}
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
