#include "gfx/bindings/InputLayout.h"
#include "gfx/GFXException.h"

gfx::InputLayout::InputLayout(
	Graphics&                                    gfx,
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
	ID3DBlob*                                    pVertexShaderBytecode)
{
	DX_HR_ERROR_TEST_AND_THROW(GetDevice(gfx)->CreateInputLayout(
		layout.data(),
		(UINT)layout.size(),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&pInputLayout));
}

void
gfx::InputLayout::Bind(Graphics& gfx)
{
	DX_CALL(GetContext(gfx)->IASetInputLayout(pInputLayout.Get()));
}