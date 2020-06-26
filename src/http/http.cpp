#include "http.hpp"

/**
 * https://stackoverflow.com/a/36401787
 * Adapted to fit codestyle
 */
size_t curlwrite_callbackfunc_stdstring(void* contents, const size_t size, const size_t nmemb, std::string* s) {
    const auto new_length = size * nmemb;
    try {
        s->append(static_cast<char*>(contents), new_length);
    } catch(std::bad_alloc &e) {
	    // handle memory problem
	    return 0;
    }
    return new_length;
}

std::string http::get(const char* url) {
	CURL* req = curl_easy_init();
	std::string s;

	curl_easy_setopt(req, CURLOPT_URL, url);
	curl_easy_setopt(req, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, curlwrite_callbackfunc_stdstring);
	curl_easy_setopt(req, CURLOPT_WRITEDATA, &s);
	/*auto res = */ curl_easy_perform(req);
	curl_easy_cleanup(req);

#ifdef _DEBUG
	printf("(http) GET %s - length %llu - str %s\n", url, s.length(), s.c_str());
#endif
	
	return s;
}

std::string http::get(const char* url, const char* token) {
	CURL* req = curl_easy_init();
	std::string s;

	curl_easy_setopt(req, CURLOPT_URL, url);
	curl_easy_setopt(req, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, curlwrite_callbackfunc_stdstring);
	curl_easy_setopt(req, CURLOPT_WRITEDATA, &s);
	struct curl_slist *chunk = nullptr;
	chunk = curl_slist_append(chunk, (std::string("Authorization: Bearer ") + token).c_str());
	chunk = curl_slist_append(chunk, "Charset: utf-8");
	chunk = curl_slist_append(chunk, "Charset: utf-8");
	curl_easy_setopt(req, CURLOPT_HTTPHEADER, chunk);
	/*auto res = */ curl_easy_perform(req);
	curl_easy_cleanup(req);

#ifdef _DEBUG
	printf("(http) GET %s - length %llu - str %s\n", url, s.length(), s.c_str());
#endif
	
	return s;
}


std::string http::post(const char* url, const char* data) {
	CURL* req = curl_easy_init();
	std::string s;

	curl_easy_setopt(req, CURLOPT_URL, url);
	curl_easy_setopt(req, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, curlwrite_callbackfunc_stdstring);
	curl_easy_setopt(req, CURLOPT_WRITEDATA, &s);
	curl_easy_setopt(req, CURLOPT_POSTFIELDS, data);
	struct curl_slist *chunk = nullptr;
	chunk = curl_slist_append(chunk, "Content-Type: application/json");
	chunk = curl_slist_append(chunk, "Charset: utf-8");
	curl_easy_setopt(req, CURLOPT_HTTPHEADER, chunk);
	
	/*auto res = */ curl_easy_perform(req);
	curl_easy_cleanup(req);

#ifdef _DEBUG
	printf("(http) POST %s - length %llu - str %s\n", url, s.length(), s.c_str());
#endif
	
	return s;
}
std::string http::post(const char* url, const char* data, const char* token) {
	CURL* req = curl_easy_init();
	std::string s;

	curl_easy_setopt(req, CURLOPT_URL, url);
	curl_easy_setopt(req, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, curlwrite_callbackfunc_stdstring);
	curl_easy_setopt(req, CURLOPT_WRITEDATA, &s);
	curl_easy_setopt(req, CURLOPT_POSTFIELDS, data);
	struct curl_slist *chunk = nullptr;
	chunk = curl_slist_append(chunk, "Content-Type: application/json");
	chunk = curl_slist_append(chunk, (std::string("Authorization: Bearer ") + token).c_str());
	chunk = curl_slist_append(chunk, "Charset: utf-8");
	curl_easy_setopt(req, CURLOPT_HTTPHEADER, chunk);
	
	/*auto res = */ curl_easy_perform(req);
	//curl_easy_cleanup(req);

#ifdef _DEBUG
	printf("(http) POST %s - length %llu - str %s\n", url, s.length(), s.c_str());
#endif
	
	return s;
}
