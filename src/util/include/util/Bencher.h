#pragma once

namespace util
{
	class Bencher
	{
	public:
		using Clock = std::chrono::high_resolution_clock;

		template <typename Func>
		static decltype(auto)
		Run(const std::string& name, Func&& f)
		{
			Bencher b;
			if constexpr (std::is_void_v<std::invoke_result_t<Func>>)
			{
				f();
				logger::info("Operation `{}` took {} milliseconds", name, b.GetElapsed());
			}
			else
			{
				auto result = f();
				logger::info("Operation `{}` took {} milliseconds", name, b.GetElapsed());
				return result;
			}
		}

	private:
		Bencher() noexcept;

		long long
		GetElapsed() const noexcept;

	private:
		Clock::time_point start_;
	};
}
