#include "Game.h"
#include "gfx/draw/geom/Box.h"
#include "gfx/draw/geom/Melon.h"
#include "gfx/draw/geom/Pyramid.h"

Game::Game() : wnd(1280u, 720u), gfx(1280u, 720u)
{
	class Factory
	{
	public:
		Factory(gfx::Graphics& gfx) : gfx(gfx) {}
		std::unique_ptr<gfx::Drawable>
		operator()()
		{
			namespace geom = gfx::geom;
			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<geom::Pyramid>(gfx, rng, adist, ddist, odist, rdist);
			case 1:
				return std::make_unique<geom::Box>(gfx, rng, adist, ddist, odist, rdist, bdist);
			case 2:
				return std::make_unique<
					geom::Melon>(gfx, rng, adist, ddist, odist, rdist, longdist, latdist);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}

	private:
		gfx::Graphics&                        gfx;
		std::mt19937                          rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f, util::math::PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f, util::math::PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f, util::math::PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
		std::uniform_int_distribution<int>    latdist{ 5, 20 };
		std::uniform_int_distribution<int>    longdist{ 10, 40 };
		std::uniform_int_distribution<int>    typedist{ 0, 2 };
	};

	Factory f{ gfx };
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);
	gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

void
Game::Play()
{
	while (wnd.Process())
	{
		while (auto key = wnd.kbd.ReadKey())
		{
			if (key->type == wnd::Keyboard::KeyEvent::Type::kDown)
			{
				drawables.clear();
			}
		}

		DoFrame();
	}
}

void
Game::DoFrame()
{
	const auto dt = timer.Mark();
	gfx.ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& d : drawables)
	{
		d->Update(dt);
		d->Draw(gfx);
	}
	gfx.EndFrame();
}
