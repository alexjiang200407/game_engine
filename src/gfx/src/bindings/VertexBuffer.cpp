#include "bindings/VertexBuffer.h"

gfx::VertexBuffer::VertexBuffer(
	DX11Graphics&             gfx,
	const geom::VertexBuffer& vbuf,
	std::string_view          fileName,
	std::string_view          tag) :
	Bindable(fileName, tag), stride(static_cast<UINT>(vbuf.GetLayout().Size()))
{
	D3D11_BUFFER_DESC bd      = {};
	bd.BindFlags              = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage                  = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags         = 0u;
	bd.MiscFlags              = 0u;
	bd.ByteWidth              = static_cast<UINT>(vbuf.SizeBytes());
	bd.StructureByteStride    = stride;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem                = vbuf.GetData();
	DX_HR_ERROR_TEST_AND_THROW(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
}

void
gfx::VertexBuffer::Bind(DX11Graphics& gfx)
{
	const UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}