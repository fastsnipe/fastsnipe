#pragma once
#define CURL_STATICLIB
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <filesystem>
#include <chrono>
#include <string>

#include "constants.hpp"
#include "curl/curl.h"
#include "json/json.hpp"

using json = nlohmann::json;
