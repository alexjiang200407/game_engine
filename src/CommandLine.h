#pragma once
#include "CommandRegister.h"
#include <string>

namespace ImGui
{
	class CommandLine
	{
	public:
		CommandLine() = default;

		void
		DrawControlWindow() noexcept;

		void
		ProcessPending() noexcept;

	private:
		friend class ::CommandRegister;

		std::string              buf;
		std::vector<std::string> history;
	};
}
