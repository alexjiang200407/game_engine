#include "draw/Drawable.h"

#include "DX11Graphics.h"
#include "bindings/IndexBuffer.h"
#include <cassert>
#include <typeinfo>

void
gfx::Drawable::Draw(Graphics& gfx) const
{
	for (auto& b : binds)
	{
		b->Bind(*gfx);
	}
	gfx->DrawIndexed();
}
