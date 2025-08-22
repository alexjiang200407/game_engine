#include "gfx/DrawableFactory.h"
#include "draw/Geometry.h"
#include "draw/Sheet.h"

gfx::DrawableFactory::DrawableFactory(gfx::IGraphics& gfx) noexcept : gfx(gfx) {}

std::unique_ptr<gfx::IDrawable>
gfx::DrawableFactory::operator()()
{
	namespace geom = gfx::geom;

	switch (gfx.GetRenderAPI())
	{
	case IGraphics::RenderAPI::kDX11:
		{
			auto& dxGfx = static_cast<gfx::Graphics&>(gfx);

			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<geom::Pyramid>(dxGfx, rng, adist, ddist, odist, rdist);
			case 1:
				return std::make_unique<geom::Box>(dxGfx, rng, adist, ddist, odist, rdist, bdist);
			case 2:
				return std::make_unique<
					geom::Melon>(dxGfx, rng, adist, ddist, odist, rdist, longdist, latdist);
			case 3:
				return std::make_unique<Sheet>(dxGfx, rng, adist, ddist, odist, rdist);
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