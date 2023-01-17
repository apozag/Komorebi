#pragma once

#include <string>

class WideStringUtility {
public:
	static std::wstring charToLPWSTR(const char* cs) {
		size_t size = 1000;
		size_t i;
		size_t length = strlen(cs) + 1;
		wchar_t* buffer = (wchar_t*)malloc(length * sizeof(wchar_t));
		mbsrtowcs_s(&i, buffer, length, &cs, length-1, NULL);
		return std::wstring(buffer);
	}

	static char* LPWSTRtoChar(const wchar_t* wStr) {
		size_t size = 200;
		size_t i;
		char* buffer = (char*)malloc(size);
		wcstombs_s(&i, buffer, size, wStr, size);
		return buffer;
	}
};