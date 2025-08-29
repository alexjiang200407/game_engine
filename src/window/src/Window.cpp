#include "window/Window.h"
#include "window/Win32Exception.h"
#include <array>
#include <cassert>
#include <imgui_impl_win32.h>
#include <limits>
#include <span>

extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace wnd;

Window::Window() :
	hInstance(GetModuleHandle(nullptr)), wndSettings(util::Settings::Module("Window"))
{
	unsigned int width  = wndSettings.Get("uWidth", 800u);
	unsigned int height = wndSettings.Get("uHeight", 600u);

	assert(
		width < static_cast<unsigned int>(std::numeric_limits<int>::max()) &&
		height < static_cast<unsigned int>(std::numeric_limits<int>::max()));

	WNDCLASSEX wc    = {};
	wc.cbSize        = sizeof(wc);
	wc.lpfnWndProc   = WindowProc;
	wc.hInstance     = hInstance;
	wc.lpszClassName = CLASS_NAME;

	WIN32_ERR_TEST_AND_THROW(RegisterClassEx(&wc));

	auto nameStr  = wndSettings.Get("sWindowName", "Game Engine"s);
	auto nameWStr = std::wstring(nameStr.begin(), nameStr.end());

	CreateAppWindow(hInstance, static_cast<int>(width), static_cast<int>(height), nameWStr.c_str());

	{
		POINT pt{};
		WIN32_ERR_TEST_AND_THROW(GetCursorPos(&pt));
		WIN32_ERR_TEST_AND_THROW(ScreenToClient(hWnd, &pt));

		RECT rc{};
		if (GetClientRect(hWnd, &rc) && PtInRect(&rc, pt))
		{
			mouse.state.set(MouseEvent::StateFlags::kInsideWindow);
		}
	}
	RegisterInput();

	while (::ShowCursor(FALSE) >= 0);
}

Window::~Window() noexcept
{
	if (hWnd)
	{
		assert(DestroyWindow(hWnd) != 0);
		ImGui_ImplWin32_Shutdown();
		hWnd = nullptr;
	}
	assert(UnregisterClass(CLASS_NAME, hInstance) != 0);
}

void
Window::CreateAppWindow(HINSTANCE a_hInstance, int width, int height, const wchar_t* title)
{
	format               = static_cast<Format>(wndSettings.Get("uWindowFormat", 0u));
	DWORD        style   = 0;
	DWORD        exStyle = 0;
	RECT         rect    = { 0, 0, width, height };

	switch (format)
	{
	case Format::Windowed:
		style   = WS_OVERLAPPEDWINDOW;
		exStyle = 0;
		break;

	case Format::BorderlessWindowed:
		style   = WS_POPUP | WS_VISIBLE;
		exStyle = WS_EX_APPWINDOW;
		break;

	case Format::BorderlessFullscreen:
		{
			DEVMODE dm = {};
			ChangeDisplaySettings(&dm, CDS_FULLSCREEN);

			style   = WS_POPUP;
			exStyle = WS_EX_APPWINDOW;

			rect.left   = 0;
			rect.top    = 0;
			rect.right  = GetSystemMetrics(SM_CXSCREEN);
			rect.bottom = GetSystemMetrics(SM_CYSCREEN);
		}
		break;
	}

	WIN32_ERR_TEST_AND_THROW(AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE));

	hWnd = WIN32_ERR_TEST_AND_THROW(CreateWindowEx(
		exStyle,
		CLASS_NAME,
		title,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		a_hInstance,
		this));

	// Has to be after window CreateWindowEx
	if (format == Format::BorderlessWindowed)
	{
		ClipCursor(&rect);
	}

	WIN32_ERR_TEST_AND_THROW(ShowWindow(hWnd, SW_SHOW));

	if (!ImGui_ImplWin32_Init(hWnd))
		throw std::runtime_error("Failed to start ImGui");

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
Window::Process() noexcept
{
	ImGui_ImplWin32_NewFrame();
	return ProcessMessages();
}

void
wnd::Window::EnableCursor() noexcept
{
	cursorEnabled = true;
	ShowCursor();
}

void
wnd::Window::DisableCursor() noexcept
{
	cursorEnabled = false;
	HideCursor();
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
	if (ImGui_ImplWin32_WndProcHandler(a_hWnd, uMsg, wParam, lParam))
	{
		return true;
	}

	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_DESTROY:
		return 0;

	case WM_INPUT:
		{
			// stifle window input if ImGui want input
			if (const auto& imio = ImGui::GetIO();
			    imio.WantCaptureKeyboard || imio.WantCaptureMouse)
				break;

			UINT dwSize = 0;
			GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				nullptr,
				&dwSize,
				sizeof(RAWINPUTHEADER));
			if (dwSize == 0 || dwSize == static_cast<UINT>(-1))
			{
				logger::error("Failed to GetRawInputData");
				break;
			}

			std::vector<BYTE> lpb(dwSize);
			if (const auto copied = GetRawInputData(
					reinterpret_cast<HRAWINPUT>(lParam),
					RID_INPUT,
					lpb.data(),
					&dwSize,
					sizeof(RAWINPUTHEADER));
			    copied != dwSize || copied == static_cast<UINT>(-1))
			{
				logger::error("Failed to GetRawInputData");
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
	if (inside && !mouse.HasStateFlag(MouseEvent::StateFlags::kInsideWindow))
	{
		mouse.OnMouseEnter();
	}
	else if (!inside && mouse.HasStateFlag(MouseEvent::StateFlags::kInsideWindow))
	{
		mouse.OnMouseLeave();
	}
}

void
Window::ResizeWindow(unsigned int width, unsigned int height) const noexcept
{
	SetWindowPos(hWnd, nullptr, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);

	if (format == Format::BorderlessWindowed)
	{
		RECT rc{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
		ClipCursor(&rc);
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

void
Window::HideCursor() noexcept
{
	while (::ShowCursor(FALSE) >= 0);
}

void
Window::ShowCursor() noexcept
{
	while (::ShowCursor(TRUE) < 0);
}
