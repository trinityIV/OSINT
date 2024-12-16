#pragma once
#include <string>

namespace config {
    struct APIKeys {
        static const std::string VIRUSTOTAL_API_KEY;
        static const std::string SHODAN_API_KEY;
        static const std::string TWITTER_API_KEY;
        static const std::string TWITTER_API_SECRET;
        static const std::string GITHUB_API_KEY;
        static const std::string EXPLOIT_DB_API_KEY;
        static const std::string CENSYS_API_ID;
        static const std::string CENSYS_API_SECRET;
    };

    struct APIEndpoints {
        static const std::string VIRUSTOTAL_BASE_URL;
        static const std::string SHODAN_BASE_URL;
        static const std::string TWITTER_BASE_URL;
        static const std::string GITHUB_BASE_URL;
        static const std::string EXPLOIT_DB_BASE_URL;
        static const std::string CENSYS_BASE_URL;
    };
}