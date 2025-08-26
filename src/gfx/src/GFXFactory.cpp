#include "gfx/GFXFactory.h"
#include "draw/Box.h"
#include "draw/Mesh.h"
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

std::unique_ptr<gfx::IDrawable>
gfx::GFXFactory::CreateSkinnedBox(Graphics& gfx)
{
	return std::make_unique<SkinnedBox>(*gfx, rng, adist, ddist, odist, rdist);
}

std::unique_ptr<gfx::IDrawable>
gfx::GFXFactory::CreateMesh(Graphics& gfx, const std::string& path, float scale)
{
	const DirectX::XMFLOAT3 mat = { 1.0f, 1.0f, 1.0f };
	return std::make_unique<Mesh>(*gfx, path, mat, rng, adist, ddist, odist, rdist, bdist, scale);
}