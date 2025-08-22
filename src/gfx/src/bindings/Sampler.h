#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class Graphics;

	class Sampler : public Bindable
	{
	public:
		Sampler(Graphics& gfx);

		void
		Bind(Graphics& gfx) override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	};
}