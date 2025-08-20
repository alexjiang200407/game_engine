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
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void
gfx::Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!DEBUG)
{
	assert(
		"*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

void
gfx::Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!DEBUG)
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = ibuf.get();
	binds.push_back(std::move(ibuf));
}
