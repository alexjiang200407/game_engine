#pragma once
#include <limits>
#include <math.h>

namespace util
{
	namespace math
	{
		constexpr float  PI   = 3.14159265f;
		constexpr double PI_D = 3.1415926535897932;

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

		template <typename T>
		constexpr auto
		sq(const T& x)
		{
			return x * x;
		}

		template <typename T>
		T
		wrap_angle(T theta)
		{
			const T modded = fmod(theta, (T)2.0 * (T)PI_D);
			return (modded > (T)PI_D) ? (modded - (T)2.0 * (T)PI_D) : modded;
		}

		template <typename T>
		constexpr T
		interpolate(const T& src, const T& dst, float alpha)
		{
			return src + (dst - src) * alpha;
		}

		template <typename T>
		constexpr T
		to_rad(T deg)
		{
			return deg * PI / (T)180.0;
		}
	}
}