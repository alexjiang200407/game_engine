#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class Blender : public Bindable<Blender, bool>
	{
	public:
		Blender(DX11Graphics& gfx, bool blending);

		void
		Bind(DX11Graphics& gfx) noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;
		bool                                     blending;
	};
}