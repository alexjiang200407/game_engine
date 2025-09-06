#include "bindings/ConstantBufferEx.h"

void
gfx::PixelConstantBufferEX::Update(DX11Graphics& gfx, const dcb::Buffer& buf) const
{
	assert(&buf.GetRootLayoutElement() == &GetRootLayoutElement());

	D3D11_MAPPED_SUBRESOURCE msr{};
	DX_HR_ERROR_TEST_AND_THROW(
		GetContext(gfx)->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
	memcpy(msr.pData, buf.GetData(), buf.GetSizeInBytes());
	DX_CALL(GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u));
}

void
gfx::PixelConstantBufferEX::Bind(DX11Graphics& gfx)
{
	DX_CALL(GetContext(gfx)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf()));
}

gfx::PixelConstantBufferEX::PixelConstantBufferEX(
	DX11Graphics&             gfx,
	const dcb::LayoutElement& layoutRoot,
	std::string_view          tag,
	UINT                      slot,
	const dcb::Buffer*        pBuf) : Bindable(tag), slot(slot)
{
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage               = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags           = 0u;
	cbd.ByteWidth           = (UINT)layoutRoot.GetSizeInBytes();
	cbd.StructureByteStride = 0u;

	if (pBuf != nullptr)
	{
		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem                = pBuf->GetData();
		DX_HR_ERROR_TEST_AND_THROW(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
	}
	else
	{
		DX_HR_ERROR_TEST_AND_THROW(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
	}
}

gfx::CachingPixelConstantBufferEX::CachingPixelConstantBufferEX(
	DX11Graphics&            gfx,
	const dcb::CookedLayout& layout,
	std::string_view         tag,
	UINT                     slot) :
	PixelConstantBufferEX(gfx, *layout.ShareRoot(), tag, slot, nullptr), buf(dcb::Buffer(layout))
{}

gfx::CachingPixelConstantBufferEX::CachingPixelConstantBufferEX(
	DX11Graphics&      gfx,
	std::string_view   tag,
	const dcb::Buffer& buf,
	UINT slot) : PixelConstantBufferEX(gfx, buf.GetRootLayoutElement(), tag, slot, &buf), buf(buf)
{}

const gfx::dcb::LayoutElement&
gfx::CachingPixelConstantBufferEX::GetRootLayoutElement() const noexcept
{
	return buf.GetRootLayoutElement();
}

const gfx::dcb::Buffer&
gfx::CachingPixelConstantBufferEX::GetBuffer() const noexcept
{
	return buf;
}

void
gfx::CachingPixelConstantBufferEX::Bind(DX11Graphics& gfx) noexcept
{
	if (dirty)
	{
		Update(gfx, buf);
		dirty = false;
	}
	PixelConstantBufferEX::Bind(gfx);
}

gfx::NocachePixelConstantBufferEX::NocachePixelConstantBufferEX(
	DX11Graphics&            gfx,
	const dcb::CookedLayout& layout,
	std::string_view         tag,
	UINT                     slot) :
	PixelConstantBufferEX(gfx, *layout.ShareRoot(), tag, slot, nullptr),
	pLayoutRoot(layout.ShareRoot())
{}

gfx::NocachePixelConstantBufferEX::NocachePixelConstantBufferEX(
	DX11Graphics&      gfx,
	const dcb::Buffer& buf,
	std::string_view   tag,
	UINT               slot) :
	PixelConstantBufferEX(gfx, buf.GetRootLayoutElement(), tag, slot, &buf),
	pLayoutRoot(buf.ShareLayoutRoot())
{}

const gfx::dcb::LayoutElement&
gfx::NocachePixelConstantBufferEX::GetRootLayoutElement() const noexcept
{
	return *pLayoutRoot;
}
