#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class IBindable;

	class Codex
	{
	public:
		static std::shared_ptr<IBindable>
		Get(const std::string& uid);

		template <typename T, typename... Args>
		static std::shared_ptr<IBindable>
		Insert(const std::string& uid, Args&&... args)
		{
			static_assert(std::is_base_of<IBindable, T>::value, "Inserted element must inherit IBindable");
			auto [it, inserted] =
				bindings.emplace(uid, std::make_shared<T>(std::forward<Args>(args)...));
			return it->second;
		}

		static void
		CullUnused() noexcept;

		static void
		Clear() noexcept;

	private:
		static inline std::unordered_map<std::string, std::shared_ptr<IBindable>> bindings;
	};
}