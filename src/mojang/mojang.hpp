#pragma once
#include "../dependencies/common_includes.hpp"

namespace mojang {
	extern std::string last_error;
	bool name_taken(std::string name);
	std::string get_uuid_for_prevname(std::string prevname);
	int64_t get_time_of_change(const std::string uuid, const std::string name);
	void change_name(const std::string uuid, const std::string password, const std::string token, const std::string name);
	bool got_name(const std::string wantedName, const std::string token);
	bool validate(const std::string token);
}