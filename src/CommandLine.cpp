#include "CommandLine.h"
#include <imgui.h>
#include <util/CommandRegister.h>

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
		ImGui::Begin("Terminal");
		ImGui::BeginChild(
			"HistoryRegion",
			ImVec2(0, -ImGui::GetFrameHeightWithSpacing()),
			false,
			ImGuiWindowFlags_HorizontalScrollbar);

		for (const auto& [line, isError] : history)
		{
			if (isError)
				ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "%s", line.c_str());
			else
				ImGui::TextWrapped("%s", line.c_str());
		}

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
		ImGui::EndChild();


		ImGui::Text(">>");
		ImGui::SameLine();
		ImGui::PushItemWidth(-FLT_MIN);

		ImGuiInputTextFlags flags =
			ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory;

		auto callback = [](ImGuiInputTextCallbackData* data) -> int {
			CommandLine* console = reinterpret_cast<CommandLine*>(data->UserData);

			if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
			{
				if (data->EventKey == ImGuiKey_UpArrow)
				{
					if (console->historyIdx == -1)
						console->historyIdx = static_cast<int>(console->history.size()) - 1;
					else if (console->historyIdx > 0)
						console->historyIdx--;

					if (console->historyIdx >= 0 &&
					    console->historyIdx < (int)console->history.size())
						data->DeleteChars(0, data->BufTextLen),
							data->InsertChars(
								0,
								console->history[console->historyIdx].first.c_str());
				}
				else if (data->EventKey == ImGuiKey_DownArrow)
				{
					if (console->historyIdx != -1 &&
					    console->historyIdx < (int)console->history.size() - 1)
						console->historyIdx++;
					else
						console->historyIdx = -1;

					data->DeleteChars(0, data->BufTextLen);
					if (console->historyIdx >= 0)
						data->InsertChars(0, console->history[console->historyIdx].first.c_str());
				}
			}
			return 0;
		};

		if (InputText("##command", &buf, flags, callback, this))
		{
			try
			{
				util::CommandRegister::Execute(buf);
				history.emplace_back(buf, false);
			}
			catch (const std::exception& e)
			{
				history.emplace_back(e.what(), true);
			}
			catch (...)
			{
				history.emplace_back("Unknown exception", true);
			}

			buf.clear();
			historyIdx = -1;

			ImGui::SetKeyboardFocusHere(-1);
		}

		ImGui::End();
	}

	void
	CommandLine::ProcessPending() noexcept
	{
		try
		{
			util::CommandRegister::Process();
		}
		catch (const std::exception& e)
		{
			history.emplace_back(std::string("Command threw: ") + e.what(), true);
		}
		catch (...)
		{
			history.emplace_back("Command threw unknown exception", true);
		}
	}
}
