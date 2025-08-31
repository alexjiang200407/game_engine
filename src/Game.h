#pragma once

#include "ImGuiManager.h"
#include <gfx/GFXFactory.h>
#include <gfx/Graphics.h>
#include <gfx/IModel.h>
#include <scene/Camera.h>
#include <util/Timer.h>
#include <window/Window.h>
#include "CommandRegister.h"
#include "CommandLine.h"

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
	gfx::GFXFactory        factory;
	ImGuiManager           imgui;  // Must be before window and graphics
	wnd::Window            wnd;
	gfx::Graphics          gfx;
	ImGui::CommandLine     cmdLine;
	util::Timer            timer;

	bool drawDebugUI = true;

	std::shared_ptr<scene::Camera>    camera;
	std::unique_ptr<gfx::IPointLight> light;
	std::unique_ptr<gfx::IModel>      pModel;
	static constexpr size_t           nDrawables = 180;
};
