#include "Game.h"
#include <util/CommandRegister.h>
#include <util/str.h>

void
Game::ResizeWindow(Game& g, unsigned int width, unsigned int height)
{
	if (width < 800u || height < 600u)
		throw std::runtime_error("width or height invalid");

	g.wnd.ResizeWindow(width, height);
	g.gfx.ResizeBuffers(width, height);
}

Game::Game() :
	light(std::move(factory.CreatePointLight(gfx))),
	pModel(std::move(factory.CreateModel(gfx, "assets/meshes/normaltest/gobber/GoblinX.obj"))),
	pModel2(std::move(factory.CreateModel(gfx, "assets/meshes/normaltest/gobber/GoblinX.obj")))
{
	gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	gfx.SetCamera(DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f));
	camera = wnd.kbd.RegisterConsumer<scene::Camera>();
	wnd.mouse.RegisterConsumer(camera);

	pModel2->SetRootTransform(DirectX::XMMatrixTranslation(0.0f, 0.0f, -25.0f));

	RegisterCommand("ResizeWindow", ResizeWindow);
}

void
Game::Play()
{
	while (wnd.Process())
	{
		if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent, false))
		{
			imgui.ToggleVisibility();
		}

		if (!imgui.IsShowing())
		{
			wnd.kbd.DispatchInputEvents();
			wnd.mouse.DispatchInputEvents();
		}
		else
		{
			wnd.kbd.Clear();
			wnd.mouse.Clear();
		}

		DoFrame();
		cmdLine.ProcessPending();
	}
}

void
Game::DoFrame()
{
	gfx.StartFrame();

	ImGui::NewFrame();

	gfx.ClearBuffer(0.0f, 0.0f, 0.0f);

	gfx.SetCamera(camera->GetMatrix());

	light->Bind(gfx, camera->GetMatrix());

	if (imgui.IsShowing())
	{
		if (ImGui::Begin("Framerate"))
		{
			ImGui::Text(
				"framerate %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);
		}
		ImGui::End();
		light->DrawControlWindow();

		pModel->DrawControlPanel(gfx, "Model 1"_csv);
		pModel2->DrawControlPanel(gfx, "Model 2"_csv);

		cmdLine.DrawControlWindow();
	}
	pModel->Draw(gfx);
	pModel2->Draw(gfx);

	light->Draw(gfx);

	gfx.EndFrame();
}