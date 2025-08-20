#pragma once
#include "gfx/bind/Bindable.h"

namespace gfx
{
	class Graphics;

	class InputLayout : public Bindable
	{
	public:
		InputLayout(
			Graphics&                                    gfx,
			const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
			ID3DBlob*                                    pVertexShaderBytecode);
		void
		Bind(Graphics& gfx) noexcept(!DEBUG) override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}
