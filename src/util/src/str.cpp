#include "util/str.h"

std::wstring
util::wtos(const std::string& value)
{
	const size_t cSize = value.size() + 1;

	std::wstring wc;
	wc.resize(cSize);

	size_t cSize1;
	mbstowcs_s(&cSize1, (wchar_t*)&wc[0], cSize, value.c_str(), cSize);

	wc.pop_back();

	return wc;
}