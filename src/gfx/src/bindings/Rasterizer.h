#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class Rasterizer : public Bindable<Rasterizer, bool>
	{
	public:
		Rasterizer(DX11Graphics& gfx, bool twoSided);

		void
		Bind(DX11Graphics& gfx) noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		bool                                          twoSided;
	};
}