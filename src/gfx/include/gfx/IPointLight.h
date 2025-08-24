#pragma once

namespace gfx
{
	class IGraphics;

	class IPointLight
	{
	public:
		virtual ~IPointLight() = default;

		virtual void
		DrawControlWindow() noexcept = 0;

		virtual void
		Reset() noexcept = 0;

		virtual void
		Draw(IGraphics& gfx) const = 0;

		virtual void
		Bind(IGraphics& gfx) const = 0;
	};
}