#pragma once
#include "CommandRegister.h"

namespace util
{
	class CommandRegister;

	template <typename T>
	class CommandContext
	{
	public:
		~CommandContext()
		{
			for (const auto& id : ids)
			{
				CommandRegister::Instance().Unregister(id);
			}
		}

	protected:
		template <typename... Args>
		void
		RegisterCommand(const std::string& id, void (*cmd)(T&, Args...))
		{
			// only thing modified is T rest of args are pure
			ids.push_back(id);

			// Capture reference to T
			CommandRegister::Instance().Register(id, [cmd, this](Args... args) {
				cmd(*static_cast<T*>(this), std::forward<Args>(args)...);
			});
		}

	private:
		std::vector<std::string> ids;
	};

};
