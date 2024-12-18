#pragma once
#include <string>
#include "http_client.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class DiscordAnalyzer {
private:
    HttpClient client;
    std::string token;

public:
    DiscordAnalyzer(const std::string& token) : token(token) {}

    const std::string& getDiscordToken() const {
        return token;
    }

    json getUserInfo(const std::string& userId) {
        std::string url = "https://discord.com/api/v10/users/" + userId;
        return client.get(url);
    }

    json getServerInfo(const std::string& serverId) {
        std::string url = "https://discord.com/api/v10/guilds/" + serverId;
        return client.get(url);
    }

    bool isValidToken() {
        try {
            std::string url = "https://discord.com/api/v10/users/@me";
            std::string response = client.get(url);
            return !response.empty();
        }
        catch (...) {
            return false;
        }
    }
};
