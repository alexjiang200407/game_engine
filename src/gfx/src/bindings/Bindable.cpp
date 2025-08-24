#include "bindings/Bindable.h"
#include "DX11Graphics.h"

ID3D11DeviceContext*
gfx::Bindable::GetContext(DX11Graphics& gfx) noexcept
{
	assert(gfx.pContext.Get());
	return gfx.pContext.Get();
}

ID3D11Device*
gfx::Bindable::GetDevice(DX11Graphics& gfx) noexcept
{
	assert(gfx.pDevice.Get());
	return gfx.pDevice.Get();
}