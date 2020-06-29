#include "proxy.hpp"
#include "../dependencies/common_includes.hpp"
#include <fstream>

namespace proxy {
	bool enabled = false;
	std::vector<std::string> proxies = {};
}

void proxy::initialize() {
	if (!std::filesystem::exists("proxies.txt")) {
		printf("[-] Proxies not found at proxies.txt.\n");
		return;
	}
	std::ifstream f("proxies.txt");
	std::string l;
	while (std::getline(f, l)) {
		proxies.push_back(l);
		printf("[*] Loaded proxy %s\n", l.c_str());
	}
	printf("[*] Loaded %llu4 proxies from proxies.txt\n", proxies.size());
	enabled = true;
}
