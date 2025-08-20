#include "gfx/bind/Topology.h"
#include "gfx/GFXException.h"

gfx::Topology::Topology(Graphics&, D3D11_PRIMITIVE_TOPOLOGY type) : type(type) {}

void
gfx::Topology::Bind(Graphics& gfx) noexcept(!DEBUG)
{
	DX_CALL(GetContext(gfx)->IASetPrimitiveTopology(type));
}