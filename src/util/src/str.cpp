#include "util/str.h"

std::wstring
util::stow(const std::string_view& value)
{
	const size_t cSize = value.size() + 1;

	std::wstring wc;
	wc.resize(cSize);

	size_t cSize1;
	mbstowcs_s(&cSize1, (wchar_t*)&wc[0], cSize, value.data(), cSize);

	wc.pop_back();

	return wc;
}

std::string
util::wtos(const std::wstring_view& value)
{
	if (value.empty())
		return {};

	int size_needed = WideCharToMultiByte(
		CP_UTF8,
		0,
		value.data(),
		(int)value.size(),
		nullptr,
		0,
		nullptr,
		nullptr);
	std::string result(size_needed, 0);
	WideCharToMultiByte(
		CP_UTF8,
		0,
		value.data(),
		(int)value.size(),
		result.data(),
		size_needed,
		nullptr,
		nullptr);
	return result;
}