#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class DX11Graphics;

	class Topology : public Bindable
	{
	public:
		Topology(DX11Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type);

		void
		Bind(DX11Graphics& gfx) override;

	protected:
		D3D11_PRIMITIVE_TOPOLOGY type;
	};
}