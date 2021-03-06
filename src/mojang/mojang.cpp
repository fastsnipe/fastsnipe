#include "mojang.hpp"
#include "../http/http.hpp"

namespace mojang {
	std::string last_error;
}

bool mojang::name_taken(std::string name) {
	if (name.length() > 16) {
		return false; // name is longer than 16 letters
	}

	const auto res = http::get((std::string("https://api.mojang.com/users/profiles/minecraft/") + std::string(name)).c_str());
	return res.length() > 0;
}
std::string mojang::get_uuid_for_prevname(std::string prevname) {
	const auto now = std::chrono::system_clock::now();
	const auto ts = (std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() - 3196800L);
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
	auto j = json::parse(res);
	auto found = false;
	auto found_idx = 0;
	printf("(dbg) get_time_of_change called\n");
	for (auto i = j.size(); i <= j.size(); --i) {
		auto obj = j[i];
		if (obj["name"].is_null())
			continue;
		if (_stricmp(obj["name"].get<std::string>().c_str(), name.c_str()) == 0) {
			found = true;
			found_idx = i + 1;
			break;
		}
		printf("%llu - %s\n", i, obj.dump().c_str());
	}
	if (!found) {
		fprintf(stderr, "[!] Failed to calculate drop-time for your wanted name.\n");
		return 0;
	}
	return j[found_idx]["changedToAt"].get<int64_t>();
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

bool mojang::got_name(const std::string wantedName, const std::string token) {
	auto mc_resp = http::get("https://api.mojang.com/user/profiles/agent/minecraft", token.c_str());
    auto mc_j = json::parse(mc_resp);
    if (!mc_j.is_array() && !mc_j["error"].is_null()) {
        // getting mc failed, error out
		fprintf(stderr, "[!] Failed to access API to get Minecraft acc info. API reported %s: %s\n", mc_j["error"].get<std::string>().c_str(), mc_j["errorMessage"].get<std::string>().c_str());
		return false;
    }
    return _stricmp(mc_j[0]["name"].get<std::string>().c_str(), wantedName.c_str()) == 0;
}

bool mojang::validate(const std::string token) {
	json j = {
		{"accessToken", token}
	};
	auto resp = http::post("https://authserver.mojang.com/validate", j.dump().c_str());
	if (resp.length() == 0) return true;
	auto j2 = json::parse(resp);
	last_error = j2["errorMessage"].get<std::string>();
	return false;
}