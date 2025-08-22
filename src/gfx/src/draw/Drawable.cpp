#include "draw/Drawable.h"
#include "GFXException.h"
#include "Graphics.h"
#include "bindings/IndexBuffer.h"
#include <cassert>
#include <typeinfo>

void
gfx::Drawable::Draw(IGraphics& gfx) const
{
	// Breaks Liskov Substitution Principle?
	if (gfx.GetRenderAPI() != IGraphics::RenderAPI::kDX11)
		throw std::runtime_error("Render API has to be directx 11");

	auto& dxGfx = static_cast<Graphics&>(gfx);

	for (auto& b : binds)
	{
		b->Bind(dxGfx);
	}
	for (auto& b : GetStaticBinds())
	{
		b->Bind(dxGfx);
	}
	dxGfx.DrawIndexed();
}
