#pragma once
#include "../dependencies/common_includes.hpp"

namespace config {
	namespace auth {
		extern std::string email;
		extern std::string password;
		extern std::string uuid;
		extern std::string token;
	}
	namespace wanted {
		extern std::string name;
		extern std::chrono::system_clock::time_point drop_time;
	}

	namespace threading {
		extern int threads;
		extern bool no_delay;
	}

	void decode(std::string cfg);
}