#include "ImGuiManager.h"

ImGuiManager::ImGuiManager() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

ImGuiManager::~ImGuiManager() noexcept { ImGui::DestroyContext(); }
