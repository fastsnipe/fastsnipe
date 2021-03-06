#include "config/config.hpp"
#include "dependencies/common_includes.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include "mojang/mojang.hpp"
#include "proxy/proxy.hpp"

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

auto DELAY = 50 / 1000; // account for time drift from mojang time

DWORD WINAPI thread_snipe(LPVOID l_param) {
	const auto thread_id = reinterpret_cast<int>(l_param);
	auto now = std::chrono::system_clock::now();
	const auto our_delay = (DELAY / config::threading::threads) * (thread_id); // likely to be changed later
	const auto p = std::chrono::system_clock::to_time_t(config::wanted::drop_time) - our_delay;
	const auto t = std::chrono::system_clock::from_time_t(p);

	while (now < t) {
		now = std::chrono::system_clock::now();
		Sleep(1); // stop cpu usage spiking
	}
	//printf("[*] Attempting to change name (attempt %i/%i)\n", thread_id + 1, config::threading::threads);
	mojang::change_name(config::auth::uuid, config::auth::password, config::auth::token, config::wanted::name);
	if (thread_id == 0) {
		// last thread to run
		if (!mojang::got_name(config::wanted::name, config::auth::token)) {
			printf("[*] Failed to acquire name. Sorry!\n");	
		} else {
			printf("[*] Acquired name \"%s\"\n", config::wanted::name.c_str());
		}
	}
	return 0;
}

int main(int argc, char* argv[]) {
	try {
		if (!search_for_config_and_load()) {
			fprintf(stderr, "[!] Failed to find and load config. Exiting.\n");
			return 1;
		}
		printf("[*] Loaded config.\nAccount email: %s\nWanted name: %s\n", config::auth::email.c_str(), config::wanted::name.c_str());

		//proxy::initialize();
		
		if (config::threading::no_delay) {
			DELAY = 0;
		}
		if (!mojang::validate(config::auth::token)) {
			fprintf(stderr, "[!] Error validating access token. Mojang returned: %s\nExiting.\n", mojang::last_error.c_str());
			return 1;
		}
		
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
		printf("drop time: %lld, s: %lld now: %lld\n", drop_time, s, now);
		if (now > s) {
			fprintf(stderr, "[!] Wanted name \"%s\" has already dropped!\n", config::wanted::name.c_str());
			return 1;
		}

		std::strftime(buf, 512, "%c", std::localtime(&t));
		printf("[*] Wanted name \"%s\" drops at %s\n", config::wanted::name.c_str(), buf);
		config::wanted::drop_time = s;
		printf("[*] Starting %i threads\n", config::threading::threads);
		for (auto i = 0; i < config::threading::threads; i++) {
			printf("[+] Started thread %i\n", i + 1);
			CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&thread_snipe), reinterpret_cast<LPVOID>(i), 0, 0);
		}
		printf("[*] Press enter to exit (do not press anything until it attempts to snipe!)\n");
		std::cin.get();
	} catch(std::exception &ex) {
		fprintf(stderr, "[!] An error occurred in the main process\n%s\n\nExiting.\n", ex.what());
		return 1;
	}
}
