#include "api_config.h"

namespace config {
    // API Keys - À remplacer par vos propres clés
    const std::string APIKeys::VIRUSTOTAL_API_KEY = "your_virustotal_key";
    const std::string APIKeys::SHODAN_API_KEY = "your_shodan_key";
    const std::string APIKeys::TWITTER_API_KEY = "your_twitter_key";
    const std::string APIKeys::TWITTER_API_SECRET = "your_twitter_secret";
    const std::string APIKeys::GITHUB_API_KEY = "your_github_key";
    const std::string APIKeys::EXPLOIT_DB_API_KEY = "your_exploit_db_key";
    const std::string APIKeys::CENSYS_API_ID = "your_censys_id";
    const std::string APIKeys::CENSYS_API_SECRET = "your_censys_secret";

    // API Endpoints
    const std::string APIEndpoints::VIRUSTOTAL_BASE_URL = "https://www.virustotal.com/vtapi/v2/";
    const std::string APIEndpoints::SHODAN_BASE_URL = "https://api.shodan.io/";
    const std::string APIEndpoints::TWITTER_BASE_URL = "https://api.twitter.com/2/";
    const std::string APIEndpoints::GITHUB_BASE_URL = "https://api.github.com/";
    const std::string APIEndpoints::EXPLOIT_DB_BASE_URL = "https://www.exploit-db.com/api/";
    const std::string APIEndpoints::CENSYS_BASE_URL = "https://search.censys.io/api/v2/";
}