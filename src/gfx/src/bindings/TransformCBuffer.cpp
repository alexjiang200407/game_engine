#include "bindings/TransformCBuffer.h"

gfx::TransformCBuffer::TransformCBuffer(Graphics& gfx, const Drawable& parent) : parent(parent)
{
	if (!pVcbuf)
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx);
	}
}

void
gfx::TransformCBuffer::Bind(Graphics& gfx)
{
	const auto       model = parent.GetTransformXM();
	const Transforms tf    = { DirectX::XMMatrixTranspose(model),
		                       DirectX::XMMatrixTranspose(
                                model * gfx.GetCamera() * gfx.GetProjection()) };

	pVcbuf->Update(gfx, tf);
	pVcbuf->Bind(gfx);
}
