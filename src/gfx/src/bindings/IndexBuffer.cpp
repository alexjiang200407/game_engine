#include "gfx/bindings/IndexBuffer.h"
#include "gfx/GFXException.h"

gfx::IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices) :
	count((UINT)indices.size())
{
	D3D11_BUFFER_DESC ibd      = {};
	ibd.BindFlags              = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage                  = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags         = 0u;
	ibd.MiscFlags              = 0u;
	ibd.ByteWidth              = static_cast<UINT>(count * sizeof(unsigned short));
	ibd.StructureByteStride    = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem                = indices.data();
	DX_HR_ERROR_TEST_AND_THROW(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &pIndexBuffer));
}

void
gfx::IndexBuffer::Bind(Graphics& gfx)
{
	DX_CALL(GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));
}

UINT
gfx::IndexBuffer::GetCount() const noexcept
{
	return count;
}
