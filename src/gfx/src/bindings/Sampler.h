#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class DX11Graphics;

	class Sampler : public Bindable
	{
	public:
		Sampler(DX11Graphics& gfx);

		void
		Bind(DX11Graphics& gfx) override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	};
}