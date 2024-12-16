#include "api_client.h"
#include <stdexcept>

namespace api {
    APIClient::APIClient() {
        curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize CURL");
        }
    }

    APIClient::~APIClient() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }

    size_t APIClient::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        userp->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    std::string APIClient::get(const std::string& url, const std::vector<std::string>& headers) {
        std::string response;
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
        struct curl_slist* chunk = nullptr;
        for (const auto& header : headers) {
            chunk = curl_slist_append(chunk, header.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        
        CURLcode res = curl_easy_perform(curl);
        if (chunk) {
            curl_slist_free_all(chunk);
        }
        
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }
        
        return response;
    }

    // Implémentation de VirusTotalAPI
    std::string VirusTotalAPI::scanFile(const std::string& filepath) {
        std::vector<std::string> headers = {
            "apikey: " + config::APIKeys::VIRUSTOTAL_API_KEY
        };
        return client->post(config::APIEndpoints::VIRUSTOTAL_BASE_URL + "file/scan", filepath, headers);
    }

    // Implémentation de ShodanAPI
    std::string ShodanAPI::hostLookup(const std::string& ip) {
        std::vector<std::string> headers = {
            "key: " + config::APIKeys::SHODAN_API_KEY
        };
        return client->get(config::APIEndpoints::SHODAN_BASE_URL + "shodan/host/" + ip, headers);
    }

    // Implémentation de TwitterAPI
    std::string TwitterAPI::getAccessToken() {
        std::vector<std::string> headers = {
            "Authorization: Basic " + config::APIKeys::TWITTER_API_KEY + ":" + config::APIKeys::TWITTER_API_SECRET
        };
        return client->post(config::APIEndpoints::TWITTER_BASE_URL + "oauth2/token", "grant_type=client_credentials", headers);
    }

    // Implémentation de GithubAPI
    std::string GithubAPI::searchUsers(const std::string& username) {
        std::vector<std::string> headers = {
            "Authorization: token " + config::APIKeys::GITHUB_API_KEY
        };
        return client->get(config::APIEndpoints::GITHUB_BASE_URL + "search/users?q=" + username, headers);
    }

}