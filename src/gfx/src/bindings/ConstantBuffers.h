#pragma once
#include "GFXException.h"
#include "bindings/Bindable.h"

namespace gfx
{
	class Graphics;

	template <typename C>
	class ConstantBuffer : public Bindable
	{
	public:
		ConstantBuffer(Graphics& gfx, unsigned int slot = 0u) : slot(slot)
		{
			D3D11_BUFFER_DESC cbd{};
			cbd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage               = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags           = 0u;
			cbd.ByteWidth           = sizeof(C);
			cbd.StructureByteStride = 0u;
			DX_HR_ERROR_TEST_AND_EXIT(
				GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
		}

		ConstantBuffer(Graphics& gfx, const C& consts, unsigned int slot = 0u) : slot(slot)
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
		unsigned int                         slot;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	};

	template <typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
		using Bindable::GetContext;

	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void
		Bind(Graphics& gfx) override
		{
			DX_CALL(
				GetContext(gfx)->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf()));
		}
	};

	template <typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
		using Bindable::GetContext;

	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void
		Bind(Graphics& gfx) override
		{
			DX_CALL(
				GetContext(gfx)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf()));
		}
	};
}