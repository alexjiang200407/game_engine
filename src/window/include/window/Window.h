#pragma once
#include "window/Keyboard.h"
#include "window/Mouse.h"
#include <util/Settings.h>

namespace gfx
{
	class Graphics;
}

namespace wnd
{
	class Window
	{
	public:
		Window();
		~Window() noexcept;
		Window(const Window&) = delete;

		Window&
		operator=(const Window&) = delete;

		[[nodiscard]] bool
		Process() noexcept;

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

		void
		HandleMouse(RAWMOUSE& rawMouse);

		void
		HandleKeyboard(RAWKEYBOARD& rawKeyboard);

		[[nodiscard]] bool
		ProcessMessages() noexcept;

	public:
		Keyboard kbd;
		Mouse    mouse;

	private:
		HWND                     hWnd         = nullptr;
		HINSTANCE                hInstance    = nullptr;
		static constexpr wchar_t CLASS_NAME[] = L"GameEngineWindow";
		util::Settings::Module   wndSettings;
	};
}
