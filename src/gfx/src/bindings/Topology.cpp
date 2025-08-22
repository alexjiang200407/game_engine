#include "bindings/Topology.h"
#include "GFXException.h"

gfx::Topology::Topology(Graphics&, D3D11_PRIMITIVE_TOPOLOGY type) : type(type) {}

void
gfx::Topology::Bind(Graphics& gfx)
{
	DX_CALL(GetContext(gfx)->IASetPrimitiveTopology(type));
}