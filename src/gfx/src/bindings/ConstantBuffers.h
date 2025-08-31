#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class DX11Graphics;

	template <typename Super, typename C>
	class ConstantBuffer : public Bindable<Super, unsigned int>
	{
	public:
		ConstantBuffer(DX11Graphics& gfx, unsigned int slot = 0u) :
			Bindable<Super, unsigned int>(slot), slot(slot)
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

		ConstantBuffer(DX11Graphics& gfx, const C& consts, unsigned int slot = 0u) :
			Bindable<Super, unsigned int>(slot), slot(slot)
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
		Update(DX11Graphics& gfx, const C& consts)
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			DX_HR_ERROR_TEST_AND_EXIT(
				GetContext(gfx)->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
			memcpy(msr.pData, &consts, sizeof(consts));
			DX_CALL(GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u));
		}

	private:
		using IBindable::GetContext;
		using IBindable::GetDevice;

	protected:
		unsigned int                         slot;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	};

	template <typename C>
	class VertexConstantBuffer : public ConstantBuffer<VertexConstantBuffer<C>, C>
	{
		using ConstantBuffer<VertexConstantBuffer<C>, C>::pConstantBuffer;
		using ConstantBuffer<VertexConstantBuffer<C>, C>::slot;
		using IBindable::GetContext;

	public:
		using ConstantBuffer<VertexConstantBuffer<C>, C>::ConstantBuffer;
		void
		Bind(DX11Graphics& gfx) override
		{
			DX_CALL(
				GetContext(gfx)->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf()));
		}
	};

	template <typename C>
	class PixelConstantBuffer : public ConstantBuffer<PixelConstantBuffer<C>, C>
	{
		using ConstantBuffer<PixelConstantBuffer<C>, C>::pConstantBuffer;
		using ConstantBuffer<PixelConstantBuffer<C>, C>::slot;
		using IBindable::GetContext;

	public:
		using ConstantBuffer<PixelConstantBuffer<C>, C>::ConstantBuffer;

		void
		Bind(DX11Graphics& gfx) override
		{
			DX_CALL(
				GetContext(gfx)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf()));
		}
	};
}