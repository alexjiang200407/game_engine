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
			std::lock_guard lg{ mut };
			static_assert(
				std::is_base_of<IBindable, T>::value,
				"Inserted element must inherit IBindable");

			auto ptr            = std::make_shared<T>(std::forward<Args>(args)...);
			auto [it, inserted] = bindings.emplace(uid, ptr);

			cur = bindings.begin();  // Prevent iterator invalidation

			return ptr;
		}

		static void
		CullUnused(size_t elements = 256u) noexcept;

		static void
		Clear() noexcept;

	private:
		using Map = std::unordered_map<std::string, std::weak_ptr<IBindable>>;

		static inline std::mutex    mut;
		static inline Map           bindings;
		static inline Map::iterator cur = bindings.end();
	};
}