#include "gfx/GFXException.h"
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

void
gfx::Graphics::DrawTestTriangle()
{
	struct Vertex
	{
		float x, y;
	};

	const Vertex vertices[] = { { 0.0f, 0.5f }, { 0.5f, -0.5f }, { -0.5f, -0.5f } };

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC         desc{};
	D3D11_SUBRESOURCE_DATA    sd{};

	{
		desc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
		desc.Usage               = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags      = 0;
		desc.MiscFlags           = 0;
		desc.ByteWidth           = sizeof(vertices);
		desc.StructureByteStride = sizeof(Vertex);
	}

	{
		sd.pSysMem = vertices;
	}

	GFX_ERROR_TEST_AND_THROW(
		pDevice->CreateBuffer(&desc, &sd, pVertexBuffer.GetAddressOf()),
		dxgiInfoManager);

	static const UINT stride = sizeof(Vertex);
	static const UINT offset = 0u;

	GFX_ERROR(
		pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset),
		dxgiInfoManager);

	// Create Pixel Shader
	wrl::ComPtr<ID3DBlob> pBlob;
	{
		wrl::ComPtr<ID3D11PixelShader> pPixelShader;

		GFX_ERROR_TEST_AND_THROW(
			D3DReadFileToBlob(L"shaders/ps_test.cso", &pBlob),
			dxgiInfoManager);

		GFX_ERROR_TEST_AND_THROW(
			pDevice->CreatePixelShader(
				pBlob->GetBufferPointer(),
				pBlob->GetBufferSize(),
				nullptr,
				&pPixelShader),
			dxgiInfoManager);

		GFX_ERROR(pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u), dxgiInfoManager);
	}

	// Create Vertex Shader
	{
		wrl::ComPtr<ID3D11VertexShader> pVertexShader;

		GFX_ERROR_TEST_AND_THROW(
			D3DReadFileToBlob(L"shaders/vs_test.cso", &pBlob),
			dxgiInfoManager);

		GFX_ERROR_TEST_AND_THROW(
			pDevice->CreateVertexShader(
				pBlob->GetBufferPointer(),
				pBlob->GetBufferSize(),
				nullptr,
				&pVertexShader),
			dxgiInfoManager);

		GFX_ERROR(pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u), dxgiInfoManager);
	}

	{
		wrl::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "Position", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u }
		};
		GFX_ERROR_TEST_AND_THROW(
			pDevice->CreateInputLayout(
				layout,
				static_cast<UINT>(std::size(layout)),
				pBlob->GetBufferPointer(),
				pBlob->GetBufferSize(),
				&pInputLayout),
			dxgiInfoManager);

		GFX_ERROR(pContext->IASetInputLayout(pInputLayout.Get()), dxgiInfoManager);
	}

	GFX_ERROR(pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr), dxgiInfoManager);

	GFX_ERROR(
		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
		dxgiInfoManager);

	// Setup Viewport
	{
		D3D11_VIEWPORT viewport{};
		viewport.Width    = static_cast<FLOAT>(width);
		viewport.Height   = static_cast<FLOAT>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		pContext->RSSetViewports(1u, &viewport);
	}

	GFX_ERROR(pContext->Draw(static_cast<UINT>(std::size(vertices)), 0u), dxgiInfoManager);
}

gfx::Graphics::Graphics(int a_width, int a_height) : width(a_width), height(a_height)
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

	GFX_ERROR_TEST_AND_THROW(
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
			&pContext),
		dxgiInfoManager);

	wrl::ComPtr<ID3D11Resource> pBackBuffer{};
	GFX_ERROR_TEST_AND_THROW(pSwap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)), dxgiInfoManager);
	GFX_ERROR_TEST_AND_THROW(
		pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget),
		dxgiInfoManager);
}
