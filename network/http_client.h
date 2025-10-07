#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>

std::string fetch_url(const std::string& url);
size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* userp);

#endif