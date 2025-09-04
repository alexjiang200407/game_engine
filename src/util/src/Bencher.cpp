#include "util/Bencher.h"

util::Bencher::Bencher() noexcept : start_(Clock::now()) {}

long long
util::Bencher::GetElapsed() const noexcept
{
	auto end     = Clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
	return elapsed;
}