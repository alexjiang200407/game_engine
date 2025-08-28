#pragma once
namespace util
{
	template <typename T>
	bool
	parseStr(const char* str, T& buf)
	{
		std::istringstream iss(str);
		iss >> buf;

		bool success = iss.eof() && !iss.fail();

		return success;
	}

#define JOIN_V(arr) join_array<sizeof(arr) / sizeof(const std::string_view), arr>::value

}
