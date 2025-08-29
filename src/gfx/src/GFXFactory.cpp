#include "gfx/GFXFactory.h"
#include "draw/Mesh.h"
#include "draw/Model.h"
#include "lighting/PointLight.h"

gfx::GFXFactory::GFXFactory() noexcept {}

std::unique_ptr<gfx::IPointLight>
gfx::GFXFactory::CreatePointLight(Graphics& gfx)
{
	const DirectX::XMFLOAT3 mat = { cdist(rng), cdist(rng), cdist(rng) };
	return std::make_unique<PointLight>(*gfx);
}

std::unique_ptr<gfx::IModel>
gfx::GFXFactory::CreateModel(Graphics& gfx, const std::string& path)
{
	return std::make_unique<gfx::Model>(gfx, path);
}