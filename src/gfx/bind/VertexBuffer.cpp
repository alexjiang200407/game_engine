#include "gfx/bind/VertexBuffer.h"

void
gfx::VertexBuffer::Bind(Graphics& gfx) noexcept(!DEBUG)
{
	const UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}