#include "window/win32/Window.h"
#include <array>
#include <span>

Window::Window(HINSTANCE a_hInstance, int width, int height, const wchar_t* title) :
	hInstance(a_hInstance)
{
	WNDCLASSEX wc    = {};
	wc.cbSize        = sizeof(wc);
	wc.lpfnWndProc   = WindowProc;
	wc.hInstance     = hInstance;
	wc.lpszClassName = CLASS_NAME;

	if (!RegisterClassEx(&wc))
	{
		throw std::runtime_error("Failed to register window class");
	}
	CreateAppWindow(hInstance, width, height, title);

	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);
		mouse.SetPos(pt.x, pt.y);
	}
	RegisterInput();
}

Window::~Window() noexcept
{
	if (hWnd)
	{
		DestroyWindow(hWnd);
		hWnd = nullptr;
	}
	UnregisterClass(CLASS_NAME, hInstance);
}

void
Window::CreateAppWindow(HINSTANCE a_hInstance, int width, int height, const wchar_t* title)
{
	RECT rect = { 0, 0, width, height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindowEx(
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
		this);

	if (!hWnd)
	{
		DWORD err = GetLastError();
		throw std::runtime_error("Failed to create window, error code: " + std::to_string(err));
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
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

	if (!RegisterRawInputDevices(rid, 2, sizeof(rid[0])))
	{
		DWORD err = GetLastError();
		throw std::runtime_error(
			"Failed to register raw input devices. Error: " + std::to_string(err));
	}
}

LRESULT CALLBACK
Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const              pWnd    = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(
			hWnd,
			GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(&Window::HandleMessageStatic));
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
			if (dwSize == 0)
				break;

			std::vector<BYTE> lpb(dwSize);
			if (GetRawInputData(
					reinterpret_cast<HRAWINPUT>(lParam),
					RID_INPUT,
					lpb.data(),
					&dwSize,
					sizeof(RAWINPUTHEADER)) != dwSize)
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

	POINT pt;
	GetCursorPos(&pt);
	RECT rc;
	GetClientRect(hWnd, &rc);
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<LPPOINT>(&rc), 2);

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
Window::ProcessMessages() noexcept
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
