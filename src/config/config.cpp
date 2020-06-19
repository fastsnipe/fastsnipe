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
	}
}

void config::decode(const std::string cfg) {
	auto j = json::parse(cfg);
	if (!j["auth"].is_null()) {
		if (!j["auth"]["email"].is_null()) auth::email = j["auth"]["email"].get<std::string>();
		if (!j["auth"]["password"].is_null()) auth::password = j["auth"]["password"].get<std::string>();
		if (!j["auth"]["uuid"].is_null()) auth::uuid = j["auth"]["uuid"].get<std::string>();
		if (!j["auth"]["token"].is_null()) auth::token = j["auth"]["token"].get<std::string>();
	}
	if (!j["wanted"].is_null()) {
		if (!j["wanted"]["name"].is_null()) wanted::name = j["wanted"]["name"].get<std::string>();
	}
	if (!j["threading"].is_null()) {
		if (!j["threading"]["threads"].is_null()) threading::threads = j["threading"]["threads"].get<int>();
	}
}