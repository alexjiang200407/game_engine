#include "gfx/DrawableFactory.h"
#include "draw/Box.h"
#include "draw/Melon.h"
#include "draw/Pyramid.h"
#include "draw/Sheet.h"
#include "draw/SkinnedBox.h"

gfx::DrawableFactory::DrawableFactory(gfx::IGraphics& gfx) noexcept : gfx(gfx) {}

std::unique_ptr<gfx::IDrawable>
gfx::DrawableFactory::operator()()
{
	switch (gfx.GetRenderAPI())
	{
	case IGraphics::RenderAPI::kDX11:
		{
			auto& dxGfx = static_cast<gfx::Graphics&>(gfx);

			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(dxGfx, rng, adist, ddist, odist, rdist);
			case 1:
				return std::make_unique<Box>(dxGfx, rng, adist, ddist, odist, rdist, bdist);
			case 2:
				return std::make_unique<
					Melon>(dxGfx, rng, adist, ddist, odist, rdist, longdist, latdist);
			case 3:
				return std::make_unique<Sheet>(dxGfx, rng, adist, ddist, odist, rdist);
			case 4:
				return std::make_unique<SkinnedBox>(dxGfx, rng, adist, ddist, odist, rdist);

			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
		break;
	default:
		throw std::runtime_error("Invalid renderer");
	}
}