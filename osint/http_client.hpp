#pragma once
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class HttpClient {
private:
    CURL* curl;
    
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        userp->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

public:
    HttpClient() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize CURL");
        }
    }

    ~HttpClient() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
    }

    json get(const std::string& url) {
        if (!curl) {
            throw std::runtime_error("CURL not initialized");
        }

        std::string response;
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
        CURLcode res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            throw std::runtime_error(std::string("CURL request failed: ") + 
                                   curl_easy_strerror(res));
        }

        try {
            return json::parse(response);
        }
        catch (const json::parse_error& e) {
            throw std::runtime_error(std::string("Failed to parse JSON response: ") + 
                                   e.what());
        }
    }

    json post(const std::string& url, const json& data) {
        if (!curl) {
            throw std::runtime_error("CURL not initialized");
        }

        std::string response;
        std::string postData = data.dump();
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        CURLcode res = curl_easy_perform(curl);
        
        curl_slist_free_all(headers);
        
        if (res != CURLE_OK) {
            throw std::runtime_error(std::string("CURL request failed: ") + 
                                   curl_easy_strerror(res));
        }

        try {
            return json::parse(response);
        }
        catch (const json::parse_error& e) {
            throw std::runtime_error(std::string("Failed to parse JSON response: ") + 
                                   e.what());
        }
    }
};
