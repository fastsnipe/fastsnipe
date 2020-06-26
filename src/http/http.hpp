#pragma once
#include "../dependencies/common_includes.hpp"
// http methods
// using libcurl

namespace http {
	std::string get(const char* url);
	std::string get(const char* url, const char* token);
	std::string post(const char* url, const char* data);
	std::string post(const char* url, const char* data, const char* token);
}