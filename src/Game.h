#pragma once

#include "ImGuiManager.h"
#include <gfx/GFXFactory.h>
#include <gfx/Graphics.h>
#include <gfx/IDrawable.h>
#include <scene/Camera.h>
#include <util/Timer.h>
#include <window/Window.h>

class Game
{
public:
	Game();

	void
	Play();

private:
	void
	DoFrame();

private:
	std::vector<std::unique_ptr<class gfx::IDrawable>> drawables;
	static constexpr size_t                            nDrawables = 180;

	gfx::GFXFactory                   factory;
	ImGuiManager                      imgui;  // Must be before window and graphics
	util::Settings::Module            settings;
	wnd::Window                       wnd;
	gfx::Graphics                     gfx;
	util::Timer                       timer;
	scene::Camera                     camera;
	std::unique_ptr<gfx::IPointLight> light;
};
