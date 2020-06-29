#include "config.hpp"

namespace config {
	namespace auth {
		std::string email;
		std::string password;
		std::string uuid;
		std::string token;
	}
	namespace wanted {
		std::string name = "aaa";
		std::chrono::system_clock::time_point drop_time;
	}

	namespace threading {
		int threads = 10;
		bool no_delay = false;
	}
}

void config::decode(const std::string cfg) {
	auto j = json::parse(cfg);
	if (j.contains("auth")) {
		if (j["auth"].contains("email")) auth::email = j["auth"]["email"].get<std::string>();
		if (j["auth"].contains("password")) auth::password = j["auth"]["password"].get<std::string>();
		if (j["auth"].contains("uuid")) auth::uuid = j["auth"]["uuid"].get<std::string>();
		if (j["auth"].contains("token")) auth::token = j["auth"]["token"].get<std::string>();
	}
	if (j.contains("wanted")) {
		if (j["wanted"].contains("name")) wanted::name = j["wanted"]["name"].get<std::string>();
	}
	if (j.contains("threading")) {
		if (j["threading"].contains("threads")) threading::threads = j["threading"]["threads"].get<int>();
		if (j["threading"].contains("no_delay")) threading::no_delay = j["threading"]["no_delay"].get<bool>();
	}
}