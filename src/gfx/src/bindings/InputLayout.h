#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class DX11Graphics;

	class InputLayout : public Bindable
	{
	public:
		InputLayout(
			DX11Graphics&                                gfx,
			const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
			ID3DBlob*                                    pVertexShaderBytecode);
		void
		Bind(DX11Graphics& gfx) override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}
