#include "SkyeFun.h"
#include <string>
#include <sstream>


std::wstring sf::wstr(const char* str) {
	std::string buff = str;
	std::wstring res = std::wstring(buff.begin(), buff.end());
	return res;
}