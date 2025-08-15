#include "window/Window.h"
#include "window/Win32Exception.h"
#include <array>
#include <span>

using namespace wnd;

Window::Window(int width, int height) : hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc    = {};
	wc.cbSize        = sizeof(wc);
	wc.lpfnWndProc   = WindowProc;
	wc.hInstance     = hInstance;
	wc.lpszClassName = CLASS_NAME;

	WIN32_ERR_TEST_AND_THROW(RegisterClassEx(&wc));
	CreateAppWindow(hInstance, width, height, L"Game Engine");  // TODO: Settings to get Window Name

	{
		POINT pt;
		WIN32_ERR_TEST_AND_THROW(GetCursorPos(&pt));
		WIN32_ERR_TEST_AND_THROW(ScreenToClient(hWnd, &pt));
		mouse.SetPos(pt.x, pt.y);
	}
	RegisterInput();
}

Window::~Window() noexcept
{
	if (hWnd)
	{
		assert(DestroyWindow(hWnd) != 0);
		hWnd = nullptr;
	}
	assert(UnregisterClass(CLASS_NAME, hInstance) != 0);
}

void
Window::CreateAppWindow(HINSTANCE a_hInstance, int width, int height, const wchar_t* title)
{
	RECT rect = { 0, 0, width, height };
	WIN32_ERR_TEST_AND_THROW(AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE));

	hWnd = WIN32_ERR_TEST_AND_THROW(CreateWindowEx(
		0,
		CLASS_NAME,
		title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		a_hInstance,
		this));

	WIN32_ERR_TEST_AND_THROW(ShowWindow(hWnd, SW_SHOW));
	WIN32_ERR_TEST_AND_THROW(UpdateWindow(hWnd));
}

void
Window::RegisterInput() const
{
	RAWINPUTDEVICE rid[2] = {};

	// Register mouse
	rid[0].usUsagePage = 0x01;  // Generic Desktop Controls
	rid[0].usUsage     = 0x02;  // Mouse
	rid[0].dwFlags     = 0;
	rid[0].hwndTarget  = hWnd;

	// Register keyboard
	rid[1].usUsagePage = 0x01;
	rid[1].usUsage     = 0x06;
	rid[1].dwFlags     = 0;
	rid[1].hwndTarget  = hWnd;

	WIN32_ERR_TEST_AND_THROW(RegisterRawInputDevices(rid, 2, sizeof(rid[0])));
}

bool
Window::Process()
{
	bool shouldContinue = ProcessMessages();

	if (shouldContinue)
	{
		while (const auto key = kbd.ReadKey())
		{
			if (key->code == 65)
			{
				logger::info("A was pressed");
			}
		}

		while (const auto mouseEvt = mouse.ReadEvent())
		{
			if (mouseEvt->state.all(Mouse::StateFlags::kInsideWindow) &&
			    mouseEvt->type == Mouse::Event::Type::kMove)
			{
				logger::info("Moving Inside Window");
			}
		}
	}

	return shouldContinue;
}

gfx::Graphics
wnd::Window::CreateGraphics(int width, int height) const
{
	return gfx::Graphics(hWnd, width, height);
}

LRESULT CALLBACK
Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const              pWnd    = static_cast<Window*>(pCreate->lpCreateParams);
		WIN32_ERR_TEST_AND_EXIT(
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd)));
		WIN32_ERR_TEST_AND_EXIT(SetWindowLongPtr(
			hWnd,
			GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(&Window::HandleMessageStatic)));
		return pWnd->HandleMessage(hWnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

namespace
{
	size_t
	utf16_to_utf32(std::span<const char16_t> buffer, std::span<char32_t> ret)
	{
		size_t retCnt = 0;
		size_t i      = 0;
		while (i < buffer.size() && retCnt < ret.size())
		{
			char32_t codepoint;
			char16_t current = buffer[i];

			if (current >= 0xD800 && current <= 0xDBFF && (i + 1) < buffer.size())
			{
				// high surrogate + low surrogate
				char16_t high = current;
				char16_t low  = buffer[i + 1];
				codepoint     = static_cast<char32_t>((high - 0xD800) << 10) +
				            static_cast<char32_t>(low - 0xDC00) + 0x10000;
				i += 2;
			}
			else
			{
				codepoint = current;
				++i;
			}

			ret[retCnt++] = codepoint;
		}

		return retCnt;
	}
}

LRESULT
Window::HandleMessage(HWND a_hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_DESTROY:
		return 0;

	case WM_INPUT:
		{
			UINT dwSize = 0;
			GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				nullptr,
				&dwSize,
				sizeof(RAWINPUTHEADER));
			if (dwSize == 0 || dwSize == static_cast<UINT>(-1))
				break;

			std::vector<BYTE> lpb(dwSize);
			if (const auto copied = GetRawInputData(
					reinterpret_cast<HRAWINPUT>(lParam),
					RID_INPUT,
					lpb.data(),
					&dwSize,
					sizeof(RAWINPUTHEADER));
			    copied != dwSize || copied == static_cast<UINT>(-1))
			{
				break;
			}

			RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb.data());

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				HandleMouse(raw->data.mouse);
			}
			else if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				HandleKeyboard(raw->data.keyboard);
			}
		}
		break;
	default:
		return DefWindowProc(a_hWnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(a_hWnd, uMsg, wParam, lParam);
}

void
Window::HandleMouse(RAWMOUSE& rawMouse)
{
	if (rawMouse.usFlags == MOUSE_MOVE_RELATIVE)
	{
		mouse.OnMouseMove(rawMouse.lLastX, rawMouse.lLastY);
	}

	if (rawMouse.usButtonFlags & RI_MOUSE_WHEEL)
	{
		short zDelta = static_cast<short>(rawMouse.usButtonData);
		mouse.OnWheel(zDelta);
	}

	if (rawMouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
		mouse.OnLeftDown();
	if (rawMouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
		mouse.OnLeftUp();

	if (rawMouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
		mouse.OnRightDown();
	if (rawMouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
		mouse.OnRightUp();

	if (rawMouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
		mouse.OnMiddleDown();
	if (rawMouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
		mouse.OnMiddleUp();

	POINT pt{};
	RECT  rc{};

	if (!GetCursorPos(&pt))
		return;
	if (!GetClientRect(hWnd, &rc))
		return;

	SetLastError(0);
	if (MapWindowPoints(hWnd, nullptr, reinterpret_cast<LPPOINT>(&rc), 2) == 0 &&
	    GetLastError() != 0)
		return;

	bool inside = PtInRect(&rc, pt);
	if (inside && !mouse.HasStateFlag(Mouse::StateFlags::kInsideWindow))
	{
		mouse.OnMouseEnter();
	}
	else if (!inside && mouse.HasStateFlag(Mouse::StateFlags::kInsideWindow))
	{
		mouse.OnMouseLeave();
	}
}

void
Window::HandleKeyboard(RAWKEYBOARD& kb)
{
	bool keyDown = !(kb.Flags & RI_KEY_BREAK);
	UINT vkey    = kb.VKey;

	if (keyDown)
	{
		kbd.OnKeyDown(vkey);
		BYTE keyboardState[256];
		GetKeyboardState(keyboardState);

		char16_t buffer[5] = {};
		int      count =
			ToUnicode(vkey, kb.MakeCode, keyboardState, reinterpret_cast<wchar_t*>(buffer), 4, 0);

		if (count)
		{
			std::array<char32_t, 2> utf32    = {};
			size_t                  retcount = utf16_to_utf32(buffer, utf32);
			for (size_t i = 0; i < retcount; ++i) kbd.OnChar(utf32[i]);
		}
	}
	else
		kbd.OnKeyUp(vkey);
}

LRESULT
Window::HandleMessageStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMessage(hWnd, uMsg, wParam, lParam);
}

bool
Window::ProcessMessages()
{
	MSG msg{};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}
