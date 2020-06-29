#pragma once
#include <string>
#include <vector>
namespace proxy {
	extern bool enabled;
	extern std::vector<std::string> proxies;
	void initialize(); // load proxies from disk
}