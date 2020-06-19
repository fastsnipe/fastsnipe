#include "mojang.hpp"
#include "../http/http.hpp"

bool mojang::name_taken(std::string name) {
	if (name.length() > 16) {
		return false; // name is longer than 16 letters
	}

	const auto res = http::get((std::string("https://api.mojang.com/users/profiles/minecraft/") + std::string(name)).c_str());
	return res.length() > 0;
}
std::string mojang::get_uuid_for_prevname(std::string prevname) {
	const auto now = std::chrono::system_clock::now();
	const auto ts = (std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() - 2629000L) / 1000L;
	char url[512] = "";
	sprintf_s(url, 512, "https://api.mojang.com/users/profiles/minecraft/%s?at=%lld", prevname.c_str(), ts);
	const auto res = http::get(url);
	if (res.length() == 0) {
		return "";
	}
	const auto j = json::parse(res);
	if (j["id"].is_null()) return "";
	return j["id"].get<std::string>();
}

int64_t mojang::get_time_of_change(const std::string uuid, const std::string name) {
	char url[512] = "";
	sprintf_s(url, 512, "https://api.mojang.com/user/profiles/%s/names", uuid.c_str());
	const auto res = http::get(url);
	if (res.length() == 0)
		return 0L;
	int64_t time = 0;	
	auto j = json::parse(res);
	auto found = false;
	for (auto& obj : j) {
		if (obj["name"].is_null())
			continue;
		if (found) {
			time = obj["changedToAt"].get<int64_t>();
			break;
		}
		if (obj["name"].get<std::string>() == name) {
			found = true;
		}
	} 
	return time;
}

void mojang::change_name(const std::string uuid, const std::string password, const std::string token, const std::string name) {
	char url[512] = "";
	sprintf_s(url, 512, "https://api.mojang.com/user/profile/%s/name", uuid.c_str());
	json d = {
		{"name", name},
		{"password", password}
	};
	const auto res = http::post(url, d.dump().c_str(), token.c_str());
	printf("name change resp %s\n", res.c_str());
}
