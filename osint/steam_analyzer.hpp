#pragma once
#include <string>
#include "http_client.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class SteamAnalyzer {
private:
    HttpClient client;
    std::string apiKey;

public:
    SteamAnalyzer(const std::string& apiKey) : apiKey(apiKey) {}

    json getPlayerSummaries(const std::string& steamId) {
        std::string url = "https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v2/?key=" + 
                         apiKey + "&steamids=" + steamId;
        return client.get(url);
    }

    json getOwnedGames(const std::string& steamId) {
        std::string url = "https://api.steampowered.com/IPlayerService/GetOwnedGames/v1/?key=" + 
                         apiKey + "&steamid=" + steamId + "&include_appinfo=1";
        return client.get(url);
    }

    json getPlayerBans(const std::string& steamId) {
        std::string url = "https://api.steampowered.com/ISteamUser/GetPlayerBans/v1/?key=" + 
                         apiKey + "&steamids=" + steamId;
        return client.get(url);
    }
};
