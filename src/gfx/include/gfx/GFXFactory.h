#pragma once
#include <gfx/Graphics.h>
#include <gfx/IDrawable.h>
#include <gfx/IPointLight.h>

namespace gfx
{
	class Graphics;

	class GFXFactory
	{
	public:
		GFXFactory() noexcept;

		std::unique_ptr<gfx::IDrawable>
		CreateBox(Graphics& gfx);

		std::unique_ptr<gfx::IPointLight>
		CreatePointLight(Graphics& gfx);

		std::unique_ptr<gfx::IDrawable>
		CreateSkinnedBox(Graphics& gfx);

		std::unique_ptr<gfx::IDrawable>
		CreateMesh(Graphics& gfx, const std::string& path, float scale = 1.0f);

	private:
		std::mt19937                          rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f, util::math::PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f, util::math::PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f, util::math::PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
		std::uniform_int_distribution<int>    latdist{ 5, 20 };
		std::uniform_int_distribution<int>    longdist{ 10, 40 };
		std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f };
	};
}