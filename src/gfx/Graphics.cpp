#include "gfx/GFXException.h"
#include <DirectXMath.h>
#include <gfx/Graphics.h>

namespace wrl = Microsoft::WRL;
namespace dx  = DirectX;

void
gfx::Graphics::EndFrame()
{
	pSwap->Present(1u, 0);
}

void
gfx::Graphics::ClearBuffer(float red, float green, float blue)
{
	const float color[] = { red, green, blue, 1.0f };
	DX_CALL(pContext->ClearRenderTargetView(pTarget.Get(), color));
	DX_CALL(pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u));
}

void
gfx::Graphics::DrawTestTriangle(float angle, int x, int y, int z)
{
	struct Vertex
	{
		float x, y, z;
	};

	float xNorm = static_cast<float>(x) / static_cast<float>(width) * 2 - 1;
	float yNorm = static_cast<float>(-y) / static_cast<float>(height) * 2 + 1;
	float zNorm = static_cast<float>(z) / 255.0f;

	const Vertex vertices[] = {
		// Front face (z = -1)
		{
			-1.0f,
			-1.0f,
			-1.0f,
		},  // bottom-left-front
		{
			1.0f,
			-1.0f,
			-1.0f,
		},  // bottom-right-front
		{
			-1.0f,
			1.0f,
			-1.0f,
		},  // top-left-front
		{
			1.0f,
			1.0f,
			-1.0f,
		},  // top-right-front

		// Back face (z = +1)
		{
			-1.0f,
			-1.0f,
			1.0f,
		},  // bottom-left-back
		{
			1.0f,
			-1.0f,
			1.0f,
		},  // bottom-right-back
		{
			-1.0f,
			1.0f,
			1.0f,
		},  // top-left-back
		{
			1.0f,
			1.0f,
			1.0f,
		},  // top-right-back
	};

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

	DX_HR_ERROR_TEST_AND_THROW(pDevice->CreateBuffer(&desc, &sd, pVertexBuffer.GetAddressOf()));

	static const UINT stride = sizeof(Vertex);
	static const UINT offset = 0u;

	DX_CALL(pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset));

	// Create Pixel Shader
	wrl::ComPtr<ID3DBlob> pBlob;
	{
		wrl::ComPtr<ID3D11PixelShader> pPixelShader;

		DX_HR_ERROR_TEST_AND_THROW(D3DReadFileToBlob(L"shaders/ps_test.cso", &pBlob));

		DX_HR_ERROR_TEST_AND_THROW(pDevice->CreatePixelShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pPixelShader));

		DX_CALL(pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u));
	}

	// Create Vertex Shader
	{
		wrl::ComPtr<ID3D11VertexShader> pVertexShader;

		DX_HR_ERROR_TEST_AND_THROW(D3DReadFileToBlob(L"shaders/vs_test.cso", &pBlob));

		DX_HR_ERROR_TEST_AND_THROW(pDevice->CreateVertexShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pVertexShader));

		DX_CALL(pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u));
	}

	{
		struct ConstantBuffer
		{
			dx::XMMATRIX transform;
		};

		const ConstantBuffer cb = { { dx::XMMatrixTranspose(
			dx::XMMatrixRotationZ(angle) * dx::XMMatrixRotationX(angle) *
			dx::XMMatrixTranslation(xNorm, yNorm, zNorm + 4.0f) *
			dx::XMMatrixPerspectiveLH(1.0f, aspectRatio, 0.5f, 10.0f)) } };

		wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
		D3D11_BUFFER_DESC         ibd{};
		ibd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
		ibd.Usage               = D3D11_USAGE_DYNAMIC;
		ibd.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
		ibd.MiscFlags           = 0;
		ibd.ByteWidth           = sizeof(cb);
		ibd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd{};
		csd.pSysMem = &cb;
		DX_HR_ERROR_TEST_AND_THROW(pDevice->CreateBuffer(&ibd, &csd, &pConstantBuffer));

		DX_CALL(pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));
	}

	{
		struct ConstantBuffer
		{
			struct
			{
				float r, g, b, a;
			} face_cols[6];
		};

		const ConstantBuffer cb = { {
			{ 1.0f, 0.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 0.0f, 1.0f },
			{ 0.0f, 1.0f, 1.0f, 1.0f },
		} };

		wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
		D3D11_BUFFER_DESC         ibd{};
		ibd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
		ibd.Usage               = D3D11_USAGE_DYNAMIC;
		ibd.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
		ibd.MiscFlags           = 0;
		ibd.ByteWidth           = sizeof(cb);
		ibd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd{};
		csd.pSysMem = &cb;
		DX_HR_ERROR_TEST_AND_THROW(pDevice->CreateBuffer(&ibd, &csd, &pConstantBuffer));

		DX_CALL(pContext->PSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));
	}

	{
		wrl::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "Position",
			  0u,
			  DXGI_FORMAT_R32G32B32_FLOAT,
			  0u,
			  0u,
			  D3D11_INPUT_PER_VERTEX_DATA,
			  0u },
		};

		DX_HR_ERROR_TEST_AND_THROW(pDevice->CreateInputLayout(
			layout,
			static_cast<UINT>(std::size(layout)),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout));

		DX_CALL(pContext->IASetInputLayout(pInputLayout.Get()));
	}

	DX_CALL(pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

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

	const unsigned short indices[] = { 0, 2, 1, 2, 3, 1, 1, 3, 5, 3, 7, 5, 2, 6, 3, 3, 6, 7,
		                               4, 5, 7, 4, 7, 6, 0, 4, 2, 2, 4, 6, 0, 1, 4, 1, 5, 4 };
	{
		wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
		D3D11_BUFFER_DESC         ibd{};
		ibd.BindFlags           = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage               = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags      = 0;
		ibd.MiscFlags           = 0;
		ibd.ByteWidth           = sizeof(indices);
		ibd.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd{};
		isd.pSysMem = indices;
		DX_HR_ERROR_TEST_AND_THROW(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

		DX_CALL(pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));
	}

	DX_CALL(pContext->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u));
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
	DX_HR_ERROR_TEST_AND_THROW(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

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
}
