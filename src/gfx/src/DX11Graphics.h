#pragma once
#include <gfx/Graphics.h>
#include <util/Settings.h>

namespace gfx
{
	class Bindable;

	class DX11Graphics
	{
		friend class gfx::Bindable;

	public:
		DX11Graphics();
		DX11Graphics(const DX11Graphics&) = delete;
		~DX11Graphics() noexcept;

		DX11Graphics&
		operator=(const DX11Graphics&) = delete;

		void
		StartFrame() const;

		void
		EndFrame() const;

		void
		ClearBuffer(float red, float green, float blue) const;

		void
		DrawIndexed() const;

		void
		SetProjection(DirectX::FXMMATRIX proj) noexcept;

		void
		SetNextDrawIndexCount(UINT count) noexcept;

		DirectX::XMMATRIX
		GetProjection() const noexcept;

		DirectX::XMMATRIX
		GetCamera() const noexcept;

		void
		SetCamera(DirectX::XMMATRIX a_camera) noexcept;

	private:
		DirectX::XMMATRIX                              projection{};
		DirectX::XMMATRIX                              camera{};
		D3D_FEATURE_LEVEL                              featureLevel;
		Microsoft::WRL::ComPtr<ID3D11Device>           pDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>    pContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain>         pSwap;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
		unsigned int                                   width;
		unsigned int                                   height;
		mutable unsigned int                           nextDrawIndexedCount = 0;
		util::Settings::Module                         gfxSettings;
	};
}
