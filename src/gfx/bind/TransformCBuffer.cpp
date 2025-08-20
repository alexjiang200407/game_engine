#include "gfx/bind/TransformCBuffer.h"

gfx::TransformCBuffer::TransformCBuffer(Graphics& gfx, const Drawable& parent) : parent(parent)
{
	if (!pVcbuf)
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
	}
}

void
gfx::TransformCBuffer::Bind(Graphics& gfx) noexcept(!DEBUG)
{
	pVcbuf->Update(gfx, DirectX::XMMatrixTranspose(parent.GetTransformXM() * gfx.GetProjection()));
	pVcbuf->Bind(gfx);
}
