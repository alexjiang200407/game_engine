#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class Graphics;

	class Topology : public Bindable
	{
	public:
		Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type);

		void
		Bind(Graphics& gfx) override;

	protected:
		D3D11_PRIMITIVE_TOPOLOGY type;
	};
}