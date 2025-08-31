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

	[[nodiscard]]
	std::wstring
	stow(const std::string_view& value);

	[[nodiscard]]
	std::string
	wtos(const std::wstring_view& value);

}
