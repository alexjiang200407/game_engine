#pragma once
#include "dcbuffer/DynamicConstant.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace gfx::dcb
{
	class LayoutCodex
	{
	public:
		static CookedLayout
		Resolve(RawLayout&& layout);

	private:
		static LayoutCodex&
																		Get_() noexcept;
		std::unordered_map<std::string, std::shared_ptr<LayoutElement>> map;
	};
}