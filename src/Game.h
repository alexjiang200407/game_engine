#pragma once

#include "ImGuiManager.h"
#include <gfx/GFXFactory.h>
#include <gfx/Graphics.h>
#include <gfx/IModel.h>
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
	gfx::GFXFactory factory;
	ImGuiManager    imgui;  // Must be before window and graphics
	wnd::Window     wnd;
	gfx::Graphics   gfx;
	util::Timer     timer;
	scene::Camera   camera;

	bool drawDebugUI = true;

	std::unique_ptr<gfx::IPointLight> light;
	std::unique_ptr<gfx::IModel>      pModel;
	static constexpr size_t           nDrawables = 180;
};
