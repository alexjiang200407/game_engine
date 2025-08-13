#pragma once
#include "window/Keyboard.h"

class Window
{
public:
	Window(HINSTANCE hInstance, int width, int height, const wchar_t* title);
	~Window() noexcept;
	Window(const Window&) = delete;

	Window&
	operator=(const Window&) = delete;

	[[nodiscard]] HWND
	GetHWND() const noexcept
	{
		return hWnd;
	}

	[[nodiscard]] bool
	ProcessMessages() noexcept;

private:
	static LRESULT CALLBACK
	WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT
	HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

	static LRESULT
	HandleMessageStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void
	CreateAppWindow(HINSTANCE hInstance, int width, int height, const wchar_t* title);

	void
	RegisterInput() const;

public:
	Keyboard kbd;

private:
	HWND                     hWnd         = nullptr;
	HINSTANCE                hInstance    = nullptr;
	static constexpr wchar_t CLASS_NAME[] = L"GameEngineWindow";
};
