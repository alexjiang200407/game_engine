#include "gfx/GFXFactory.h"
#include "draw/Box.h"
#include "draw/Pyramid.h"
#include "draw/SkinnedBox.h"
#include "lighting/PointLight.h"

gfx::GFXFactory::GFXFactory() noexcept {}

std::unique_ptr<gfx::IDrawable>
gfx::GFXFactory::CreateBox(IGraphics& gfx)
{
	switch (gfx.GetRenderAPI())
	{
	case IGraphics::RenderAPI::kDX11:
		{
			auto&                   dxGfx = static_cast<gfx::Graphics&>(gfx);
			const DirectX::XMFLOAT3 mat   = { cdist(rng), cdist(rng), cdist(rng) };
			return std::make_unique<Box>(dxGfx, rng, adist, ddist, odist, rdist, bdist, mat);
		}
		break;
	default:
		throw std::runtime_error("Invalid renderer");
	}
}

std::unique_ptr<gfx::IPointLight>
gfx::GFXFactory::CreatePointLight(IGraphics& gfx)
{
	switch (gfx.GetRenderAPI())
	{
	case IGraphics::RenderAPI::kDX11:
		{
			auto& dxGfx = static_cast<gfx::Graphics&>(gfx);
			return std::make_unique<PointLight>(dxGfx);
		}
		break;
	default:
		throw std::runtime_error("Invalid renderer");
	}
}