#pragma once
#include "window/Window.h"
#include <d3d11.h>
#include <wrl/client.h>

namespace wnd
{
	class Window;
}

namespace gfx
{

	class Graphics
	{
		friend class wnd::Window;

	public:
		Graphics(const Graphics&) = delete;

		Graphics&
		operator=(const Graphics&) = delete;

		void
		EndFrame();

		void
		ClearBuffer(float red, float green, float blue) noexcept;

	private:
		Graphics(HWND hWnd, int width, int height);

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>           pDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>    pContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain>         pSwap;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	};
}
