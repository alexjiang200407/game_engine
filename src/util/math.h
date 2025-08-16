#pragma once

namespace util
{
	namespace math
	{
		constexpr bool
		is_power_of_two(std::size_t n) noexcept
		{
			return n != 0 && ((n & (n - 1)) == 0);
		}
	}
}