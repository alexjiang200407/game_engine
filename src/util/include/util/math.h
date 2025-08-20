#pragma once
#include <limits>

namespace util
{
	namespace math
	{
		constexpr bool
		is_power_of_two(std::size_t n) noexcept
		{
			return n != 0 && ((n & (n - 1)) == 0);
		}

		template <typename T>
		constexpr bool
		safe_add_assign(T& lhs, T rhs) noexcept
		{
			static_assert(std::is_integral_v<T>, "T must be an integral type");

			if constexpr (std::is_signed_v<T>)
			{
				if (rhs > 0 && lhs > std::numeric_limits<T>::max() - rhs)
					return false;
				if (rhs < 0 && lhs < std::numeric_limits<T>::min() - rhs)
					return false;
			}
			else
			{
				if (lhs > std::numeric_limits<T>::max() - rhs)
					return false;
			}

			lhs += rhs;
			return true;
		}
	}
}