#include "bindings/Rasterizer.h"

gfx::Rasterizer::Rasterizer(DX11Graphics& gfx, bool a_twoSided) :
	Bindable(twoSided), twoSided(a_twoSided)
{
	D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rasterDesc.CullMode              = twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;

	DX_HR_ERROR_TEST_AND_THROW(GetDevice(gfx)->CreateRasterizerState(&rasterDesc, &pRasterizer));
}

void
gfx::Rasterizer::Bind(DX11Graphics& gfx) noexcept
{
	GetContext(gfx)->RSSetState(pRasterizer.Get());
}
