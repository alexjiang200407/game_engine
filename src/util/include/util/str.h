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

	std::wstring
	wtos(const std::string& value);

}
