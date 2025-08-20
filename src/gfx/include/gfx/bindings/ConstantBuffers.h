#pragma once
#include "gfx/GFXException.h"
#include "gfx/bindings/Bindable.h"

namespace gfx
{
	class Graphics;

	template <typename C>
	class ConstantBuffer : public Bindable
	{
	public:
		ConstantBuffer(Graphics& gfx)
		{
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage               = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags           = 0u;
			cbd.ByteWidth           = sizeof(C);
			cbd.StructureByteStride = 0u;
			DX_HR_ERROR_TEST_AND_EXIT(
				GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
		}

		ConstantBuffer(Graphics& gfx, const C& consts)
		{
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage               = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags           = 0u;
			cbd.ByteWidth           = sizeof(consts);
			cbd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem                = &consts;
			DX_HR_ERROR_TEST_AND_EXIT(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
		}

		void
		Update(Graphics& gfx, const C& consts)
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			DX_HR_ERROR_TEST_AND_EXIT(
				GetContext(gfx)->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
			memcpy(msr.pData, &consts, sizeof(consts));
			DX_CALL(GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u));
		}

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	};

	template <typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using Bindable::GetContext;

	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void
		Bind(Graphics& gfx) noexcept(!DEBUG) override
		{
			DX_CALL(GetContext(gfx)->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));
		}
	};

	template <typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using Bindable::GetContext;

	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void
		Bind(Graphics& gfx) noexcept(!DEBUG) override
		{
			DX_CALL(GetContext(gfx)->PSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));
		}
	};
}