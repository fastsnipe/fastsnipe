#include "config/config.hpp"
#include "dependencies/common_includes.hpp"
#include <fstream>
#include <sstream>
#include "mojang/mojang.hpp"

bool search_for_config_and_load() {
	if (!std::filesystem::exists("config.json")) {
		fprintf(stderr, "[!] Config could not be located at %s!\n", "config.json");
		return false;
	}
	const std::ifstream f("config.json");
	if (f.is_open()) {
		std::stringstream stream;
		stream << f.rdbuf(); //read the file
		const auto str = stream.str();
		config::decode(str);
		return true;
	}
	return false;
}

const auto DELAY = 50 / 1000; // account for time drift from mojang time

DWORD WINAPI thread_snipe(LPVOID l_param) {
	const auto thread_id = reinterpret_cast<int>(l_param);
	auto now = std::chrono::system_clock::now();
	const auto our_delay = (DELAY / config::threading::threads) * (thread_id + 1);
	const auto p = std::chrono::system_clock::to_time_t(config::wanted::drop_time) - our_delay;
	const auto t = std::chrono::system_clock::from_time_t(p);

	while (now < t) {
		now = std::chrono::system_clock::now();
	}
	printf("[*] Attempting to change name (attempt %i/%i)\n", thread_id, config::threading::threads);
	mojang::change_name(config::auth::uuid, config::auth::password, config::auth::token, config::wanted::name);
	return 0;
}

int main(int argc, char* argv[]) {
	try {
		if (!search_for_config_and_load()) {
			fprintf(stderr, "[!] Failed to find and load config. Exiting.\n");
			return 1;
		}
		printf("[*] Loaded config.\nAccount email: %s\nWanted name: %s\n", config::auth::email.c_str(), config::wanted::name.c_str());

		const auto name_taken = mojang::name_taken(config::wanted::name);
		if (name_taken) {
			fprintf(stderr, "[!] Wanted name \"%s\" is already taken.\n", config::wanted::name.c_str());
			return 1;
		}

		const auto uuid = mojang::get_uuid_for_prevname(config::wanted::name);

		const auto time = mojang::get_time_of_change(uuid, config::wanted::name);
		const auto drop_time = time + 3196800000;
		char buf[512] = "";
		const auto now = std::chrono::system_clock::now();
		const auto s = std::chrono::system_clock::from_time_t(drop_time / 1000);
		const auto t = std::chrono::system_clock::to_time_t(s);
		
		if (now > s) {
			fprintf(stderr, "[!] Wanted name \"%s\" has already dropped!\n", config::wanted::name.c_str());
			return 1;
		}

		std::strftime(buf, 512, "%c", std::localtime(&t));
		printf("[*] Wanted name \"%s\" drops at %s\n", config::wanted::name.c_str(), buf);
		config::wanted::drop_time = s;
		printf("[*] Starting %i threads\n", config::threading::threads);
		for (auto i = 0; i < config::threading::threads; i++) {
			printf("[+] Started thread %i\n", i);
			CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&thread_snipe), reinterpret_cast<LPVOID>(i), 0, 0);
		}

		while(true){}
	} catch(std::exception &ex) {
		fprintf(stderr, "[!] An error occurred in the main process\n%s\n\nExiting.\n", ex.what());
		return 1;
	}
}
