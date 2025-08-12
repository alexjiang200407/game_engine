#pragma once

#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class Window
{
public:
	Window(HINSTANCE hInstance, int width, int height, const wchar_t* title);
	~Window() noexcept;
	Window(const Window&)            = delete;
	Window& operator=(const Window&) = delete;

	HWND GetHWND() const noexcept { return hWnd_; }
	bool ProcessMessages() noexcept;

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT        HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT HandleMessageStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void CreateAppWindow(HINSTANCE hInstance, int width, int height, const wchar_t* title);

	HWND                     hWnd_        = nullptr;
	HINSTANCE                hInstance_   = nullptr;
	static constexpr wchar_t CLASS_NAME[] = L"GameEngineWindow";
};