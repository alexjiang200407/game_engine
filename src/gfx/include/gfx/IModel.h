#pragma once
#include <gfx/Graphics.h>

namespace gfx
{
	class IModel
	{
	public:
		virtual ~IModel() noexcept = default;

		virtual void
		Draw(Graphics& gfx) const noexcept = 0;

		virtual void
		DrawControlPanel(Graphics& gfx) noexcept = 0;

		virtual DirectX::XMMATRIX
		GetTransform() const noexcept = 0;
	};
}