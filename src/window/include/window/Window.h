#pragma once
#include "window/Keyboard.h"
#include "window/Mouse.h"
#include <util/Settings.h>
#include <util/CommandContext.h>

namespace gfx
{
	class Graphics;
}

namespace wnd
{
	class Window : public util::CommandContext<Window>
	{
	public:
		Window();
		~Window() noexcept;
		Window(const Window&) = delete;

		Window&
		operator=(const Window&) = delete;

		[[nodiscard]] bool
		Process() noexcept;

		void
		EnableCursor() noexcept;

		void
		DisableCursor() noexcept;

		void
		ResizeWindow(unsigned int width, unsigned int height) const noexcept;

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

		void
		HideCursor() noexcept;

		void
		ShowCursor() noexcept;

	public:
		Keyboard kbd;
		Mouse    mouse;

	private:
		enum class Format
		{
			Windowed,
			BorderlessWindowed,
			BorderlessFullscreen
		};

	private:
		HWND                         hWnd          = nullptr;
		HINSTANCE                    hInstance     = nullptr;
		static constexpr wchar_t     CLASS_NAME[]  = L"GameEngineWindow";
		bool                         cursorEnabled = false;
		util::Settings::ConfigModule wndSettings;
		Format                       format = Format::Windowed;
	};
}
