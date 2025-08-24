#include "bindings/VertexBuffer.h"

void
gfx::VertexBuffer::Bind(DX11Graphics& gfx)
{
	const UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}