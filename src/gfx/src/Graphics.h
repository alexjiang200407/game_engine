#pragma once
#include "gfx/IGraphics.h"

namespace gfx
{
	class Bindable;

	class Graphics : public gfx::IGraphics
	{
		friend class gfx::Bindable;

	public:
		Graphics(unsigned int width, unsigned int height);
		Graphics(const Graphics&) = delete;

		Graphics&
		operator=(const Graphics&) = delete;

		void
		EndFrame() const override;

		void
		ClearBuffer(float red, float green, float blue) const override;

		void
		DrawIndexed() const;

		void
		SetProjection(DirectX::FXMMATRIX proj) noexcept override;

		void
		SetNextDrawIndexCount(UINT count) noexcept;

		DirectX::XMMATRIX
		GetProjection() const noexcept;

		RenderAPI
		GetRenderAPI() const noexcept override;

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
		mutable unsigned int                           nextDrawIndexedCount = 0;
		float                                          aspectRatio;
	};
}
