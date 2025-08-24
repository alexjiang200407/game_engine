#include "bindings/TransformCBuffer.h"

gfx::TransformCBuffer::TransformCBuffer(Graphics& gfx, const Drawable& parent, unsigned int slot) :
	parent(parent)
{
	if (!pVcbuf)
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
	}
}

void
gfx::TransformCBuffer::Bind(Graphics& gfx)
{
	const auto       modelView = parent.GetTransformXM() * gfx.GetCamera();
	const Transforms tf        = { DirectX::XMMatrixTranspose(modelView),
		                           DirectX::XMMatrixTranspose(modelView * gfx.GetProjection()) };

	pVcbuf->Update(gfx, tf);
	pVcbuf->Bind(gfx);
}
