#pragma once
#include "gfx/DXGIInfoManager.h"
#include "window/Window.h"
#include <d3d11.h>

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
		Graphics(unsigned int width, unsigned int height);
		Graphics(const Graphics&) = delete;

		Graphics&
		operator=(const Graphics&) = delete;

		void
		EndFrame();

		void
		ClearBuffer(float red, float green, float blue);

		void
		DrawTestTriangle(float angle, int x, int y, int z);

	private:
		D3D_FEATURE_LEVEL                              featureLevel;
		Microsoft::WRL::ComPtr<ID3D11Device>           pDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>    pContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain>         pSwap;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
		unsigned int                                   width;
		unsigned int                                   height;
		float                                          aspectRatio;
	};
}
