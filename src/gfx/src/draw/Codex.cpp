#include "draw/Codex.h"

std::shared_ptr<gfx::IBindable>
gfx::Codex::Get(const std::string& uid)
{
	if (auto it = bindings.find(uid); it != bindings.end())
	{
		if (auto ptr = it->second.lock())
		{
			return ptr;
		}
	}
	return nullptr;
}

void
gfx::Codex::CullUnused(size_t elements) noexcept
{
	for (; cur != bindings.end() && elements > 0; --elements)
	{
		if (cur->second.expired())
		{
			cur = bindings.erase(cur);
		}
		else
		{
			++cur;
		}

		if (cur == bindings.end())
			cur = bindings.begin();
	}
}

void
gfx::Codex::Clear() noexcept
{
	bindings.clear();
}