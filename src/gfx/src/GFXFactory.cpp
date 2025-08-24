#include "gfx/GFXFactory.h"
#include "draw/Box.h"
#include "draw/Pyramid.h"
#include "draw/SkinnedBox.h"
#include "lighting/PointLight.h"

gfx::GFXFactory::GFXFactory() noexcept {}

std::unique_ptr<gfx::IDrawable>
gfx::GFXFactory::CreateBox(Graphics& gfx)
{
	const DirectX::XMFLOAT3 mat = { cdist(rng), cdist(rng), cdist(rng) };
	return std::make_unique<Box>(*gfx, rng, adist, ddist, odist, rdist, bdist, mat);
}

std::unique_ptr<gfx::IPointLight>
gfx::GFXFactory::CreatePointLight(Graphics& gfx)
{
	const DirectX::XMFLOAT3 mat = { cdist(rng), cdist(rng), cdist(rng) };
	return std::make_unique<PointLight>(*gfx);
}