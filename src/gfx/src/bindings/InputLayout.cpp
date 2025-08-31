#include "bindings/InputLayout.h"

gfx::InputLayout::InputLayout(
	DX11Graphics&             gfx,
	const geom::VertexLayout& layout,
	ID3DBlob*                 pVertexShaderBytecode) : Bindable(layout)
{
	const auto d3dLayout = layout.GetD3DLayout();
	DX_HR_ERROR_TEST_AND_THROW(GetDevice(gfx)->CreateInputLayout(
		d3dLayout.data(),
		static_cast<UINT>(d3dLayout.size()),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&pInputLayout));
}

void
gfx::InputLayout::Bind(DX11Graphics& gfx)
{
	DX_CALL(GetContext(gfx)->IASetInputLayout(pInputLayout.Get()));
}