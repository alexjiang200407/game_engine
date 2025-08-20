#include "gfx/bindings/Bindable.h"
#include "gfx/Graphics.h"

ID3D11DeviceContext*
gfx::Bindable::GetContext(Graphics& gfx) noexcept
{
	assert(gfx.pContext.Get());
	return gfx.pContext.Get();
}

ID3D11Device*
gfx::Bindable::GetDevice(Graphics& gfx) noexcept
{
	assert(gfx.pDevice.Get());
	return gfx.pDevice.Get();
}