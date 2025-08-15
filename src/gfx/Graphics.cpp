#include <gfx/Graphics.h>

namespace wrl = Microsoft::WRL;

void
gfx::Graphics::EndFrame()
{
	pSwap->Present(1u, 0);
}

void
gfx::Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}

gfx::Graphics::Graphics(HWND hWnd, int width, int height)
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
	sd.OutputWindow                       = hWnd;
	sd.Windowed                           = TRUE;
	sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags                              = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D11CreateDeviceAndSwapChain(
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
		&pContext);

	wrl::ComPtr<ID3D11Resource> pBackBuffer{};
	pSwap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

	pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget);
}
