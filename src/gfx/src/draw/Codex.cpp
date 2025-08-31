#include "draw/Codex.h"

std::shared_ptr<gfx::IBindable>
gfx::Codex::Get(const std::string& uid)
{
	if (auto it = bindings.find(uid); it != bindings.end())
	{
		return it->second;
	}
	return nullptr;
}

void
gfx::Codex::CullUnused() noexcept
{
	for (auto it = bindings.begin(); it != bindings.end(); )
	{
		if (it->second.use_count() == 1u)
		{
			it = bindings.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void
gfx::Codex::Clear() noexcept
{
	bindings.clear();
}