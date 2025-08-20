#pragma once
#include "gfx/DXGIInfoManager.h"
#include "gfx/bind/Bindable.h"
#include "gfx/bind/bind.h"
#include <d3d11.h>

namespace gfx
{
	class Bindable;

	class Graphics
	{
		friend class gfx::Bindable;

	public:
		Graphics(unsigned int width, unsigned int height);
		Graphics(const Graphics&) = delete;

		Graphics&
		operator=(const Graphics&) = delete;

		void
		EndFrame() const;

		void
		ClearBuffer(float red, float green, float blue) const;

		void
		DrawIndexed(unsigned int count) const noexcept(!DEBUG);

		void
		SetProjection(DirectX::FXMMATRIX proj) noexcept;

		DirectX::XMMATRIX
		GetProjection() const noexcept;

	private:
		DirectX::XMMATRIX                              projection;
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
