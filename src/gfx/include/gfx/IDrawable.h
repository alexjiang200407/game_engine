#pragma once
#include <gfx/Graphics.h>

namespace gfx
{
	class IGraphics;

	class IDrawable
	{
	public:
		IDrawable()                 = default;
		virtual ~IDrawable()        = default;
		IDrawable(const IDrawable&) = delete;

		IDrawable&
		operator=(const IDrawable&) = delete;

		virtual DirectX::XMMATRIX
		GetTransformXM() const noexcept = 0;

		virtual void
		Draw(Graphics& gfx) const = 0;

		virtual void
		Update(float dt) noexcept = 0;
	};
}
