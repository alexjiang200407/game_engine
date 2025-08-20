#include "gfx/Graphics.h"
#include "gfx/GFXException.h"
#include <DirectXMath.h>

namespace wrl = Microsoft::WRL;
namespace dx  = DirectX;

void
gfx::Graphics::EndFrame() const
{
	pSwap->Present(1u, 0);
}

void
gfx::Graphics::ClearBuffer(float red, float green, float blue) const
{
	const float color[] = { red, green, blue, 1.0f };
	DX_CALL(pContext->ClearRenderTargetView(pTarget.Get(), color));
	DX_CALL(pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u));
}

void
gfx::Graphics::DrawIndexed(unsigned int count) const noexcept(!DEBUG)
{
	DX_CALL(pContext->DrawIndexed(count, 0u, 0u));
}

void
gfx::Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX
gfx::Graphics::GetProjection() const noexcept
{
	return projection;
}

gfx::Graphics::Graphics(unsigned int a_width, unsigned int a_height) :
	width(a_width), height(a_height),
	aspectRatio(width ? static_cast<float>(height) / static_cast<float>(width) : 0.0f)
{
	DXGI_SWAP_CHAIN_DESC sd               = {};
	sd.BufferDesc.Width                   = static_cast<UINT>(width);
	sd.BufferDesc.Height                  = static_cast<UINT>(height);
	sd.BufferDesc.Format                  = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator   = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count                   = 1;
	sd.SampleDesc.Quality                 = 0;
	sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount                        = 1;
	sd.OutputWindow                       = GetActiveWindow();
	sd.Windowed                           = TRUE;
	sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags                              = 0;

	UINT swapCreateFlags = 0u;

#ifdef DEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DX_HR_ERROR_TEST_AND_THROW(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext));

	wrl::ComPtr<ID3D11Resource> pBackBuffer{};
	DX_HR_ERROR_TEST_AND_THROW(pSwap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));
	DX_HR_ERROR_TEST_AND_THROW(
		pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

	{
		D3D11_DEPTH_STENCIL_DESC dsDesc{};
		dsDesc.DepthEnable    = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc      = D3D11_COMPARISON_LESS;
		wrl::ComPtr<ID3D11DepthStencilState> pDSState;
		DX_HR_ERROR_TEST_AND_THROW(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));
		DX_CALL(pContext->OMSetDepthStencilState(pDSState.Get(), 1u));
	}

	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	{
		D3D11_TEXTURE2D_DESC descDepth{};
		descDepth.Width              = width;
		descDepth.Height             = height;
		descDepth.MipLevels          = 1u;
		descDepth.ArraySize          = 1u;
		descDepth.Format             = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count   = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage              = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags          = D3D11_BIND_DEPTH_STENCIL;

		DX_HR_ERROR_TEST_AND_THROW(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));
	}

	{
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV{};
		descDSV.Format             = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0u;
		DX_HR_ERROR_TEST_AND_THROW(
			pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDSV));
	}
	DX_CALL(pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get()));

	D3D11_VIEWPORT vp{};
	vp.Width    = static_cast<FLOAT>(width);
	vp.Height   = static_cast<FLOAT>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	DX_CALL(pContext->RSSetViewports(1u, &vp));
}
