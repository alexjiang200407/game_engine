#include "bindings/Topology.h"

gfx::Topology::Topology(DX11Graphics&, D3D11_PRIMITIVE_TOPOLOGY type) :
	Bindable(static_cast<int>(type)), type(type)
{}

void
gfx::Topology::Bind(DX11Graphics& gfx)
{
	DX_CALL(GetContext(gfx)->IASetPrimitiveTopology(type));
}