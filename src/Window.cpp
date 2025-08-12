#include "Window.h"
#include <stdexcept>
#include <string>

Window::Window(HINSTANCE hInstance, int width, int height, const wchar_t* title) :
	hInstance_(hInstance)
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
	CreateAppWindow(hInstance_, width, height, title);
}

Window::~Window() noexcept
{
	if (hWnd_)
	{
		DestroyWindow(hWnd_);
		hWnd_ = nullptr;
	}
	UnregisterClass(CLASS_NAME, hInstance_);
}

void Window::CreateAppWindow(HINSTANCE hInstance, int width, int height, const wchar_t* title)
{
	RECT rect = { 0, 0, width, height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd_ = CreateWindowEx(
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
		hInstance,
		this);

	if (!hWnd_)
	{
		DWORD err = GetLastError();
		throw std::runtime_error("Failed to create window, error code: " + std::to_string(err));
	}

	ShowWindow(hWnd_, SW_SHOW);
	UpdateWindow(hWnd_);
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT Window::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_DESTROY:
		return 0;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT Window::HandleMessageStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMessage(hWnd, uMsg, wParam, lParam);
}

bool Window::ProcessMessages() noexcept
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