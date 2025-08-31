#include "CommandLine.h"
#include "CommandRegister.h"
#include <imgui.h>

namespace ImGui
{

	IMGUI_API bool
	InputText(
		const char*            label,
		std::string*           str,
		ImGuiInputTextFlags    flags     = ImGuiInputTextFlags_EnterReturnsTrue,
		ImGuiInputTextCallback callback  = NULL,
		void*                  user_data = NULL);

	void
	CommandLine::DrawControlWindow() noexcept
	{
		if (ImGui::InputText("Command", &buf, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			try
			{
				CommandRegister::Instance().Execute(buf);
				history.push_back(buf);
			}
			catch (const std::exception& e)
			{
				history.push_back(e.what());
			}
			catch (...)
			{
				history.push_back("Unknown exception");
			}
			buf.clear();
		}

		for (const auto& line : history)
		{
			ImGui::TextWrapped(line.c_str());
		}
	}

	void
	CommandLine::ProcessPending() noexcept
	{
		try
		{
			CommandRegister::Instance().Process();
		}
		catch (const std::exception& e)
		{
			history.push_back(std::string("Command threw: ") + e.what());
		}
		catch (...)
		{
			history.push_back("Command threw unknown exception");
		}
	}
}
