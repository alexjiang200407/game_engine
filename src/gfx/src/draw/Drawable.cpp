#include "gfx/draw/Drawable.h"
#include "gfx/GFXException.h"
#include "gfx/Graphics.h"
#include "gfx/bindings/IndexBuffer.h"
#include <cassert>
#include <typeinfo>

void
gfx::Drawable::Draw(Graphics& gfx) const noexcept(!DEBUG)
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	for (auto& b : GetStaticBinds())
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed();
}
