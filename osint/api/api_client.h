#pragma once
#include <string>
#include <vector>
#include <memory>
#include <curl/curl.h>
#include "../config/api_config.h"

namespace api {
    class APIClient {
    public:
        APIClient();
        ~APIClient();
        
        std::string get(const std::string& url, const std::vector<std::string>& headers = {});
        std::string post(const std::string& url, const std::string& data, const std::vector<std::string>& headers = {});
        
    private:
        CURL* curl;
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    };

    class VirusTotalAPI {
    public:
        std::string scanFile(const std::string& filepath);
        std::string getFileReport(const std::string& resource);
        std::string scanUrl(const std::string& url);
        std::string getUrlReport(const std::string& resource);
    private:
        std::unique_ptr<APIClient> client;
    };

    class ShodanAPI {
    public:
        std::string hostLookup(const std::string& ip);
        std::string searchHosts(const std::string& query);
        std::string getPorts(const std::string& ip);
    private:
        std::unique_ptr<APIClient> client;
    };

    class TwitterAPI {
    public:
        std::string searchUsers(const std::string& username);
        std::string getUserTweets(const std::string& user_id);
        std::string getUserFollowers(const std::string& user_id);
    private:
        std::unique_ptr<APIClient> client;
        std::string getAccessToken();
    };

    class GithubAPI {
    public:
        std::string searchUsers(const std::string& username);
        std::string getUserRepos(const std::string& username);
        std::string getRepoContents(const std::string& repo, const std::string& path);
    private:
        std::unique_ptr<APIClient> client;
    };

    class ExploitDBAPI {
    public:
        std::string searchExploits(const std::string& query);
        std::string getExploitDetails(const std::string& id);
    private:
        std::unique_ptr<APIClient> client;
    };

    class CensysAPI {
    public:
        std::string searchHosts(const std::string& query);
        std::string getCertificates(const std::string& domain);
    private:
        std::unique_ptr<APIClient> client;
    };
}