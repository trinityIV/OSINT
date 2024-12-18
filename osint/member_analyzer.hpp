#pragma once
#include <string>
#include <vector>
#include <map>
#include "http_client.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class MemberAnalyzer {
private:
    HttpClient client;
    std::string discordToken;
    std::string steamApiKey;

public:
    MemberAnalyzer(const std::string& discordToken, const std::string& steamApiKey)
        : discordToken(discordToken), steamApiKey(steamApiKey) {}

    json getDiscordUserInfo(const std::string& userId) {
        std::string url = "https://discord.com/api/v10/users/" + userId;
        return client.get(url);
    }

    json getSteamUserInfo(const std::string& steamId) {
        std::string url = "https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v2/?key=" + 
                         steamApiKey + "&steamids=" + steamId;
        return client.get(url);
    }

    json checkSteamGroups(const std::string& steamId) {
        std::string url = "https://api.steampowered.com/ISteamUser/GetUserGroupList/v1/?key=" + 
                         steamApiKey + "&steamid=" + steamId;
        return client.get(url);
    }

    json checkDiscordGuilds(const std::string& userId) {
        std::string url = "https://discord.com/api/v10/users/" + userId + "/guilds";
        return client.get(url);
    }

    bool analyzeBehavior(const std::string& userId) {
        try {
            // Analyze user behavior patterns
            json userInfo = getDiscordUserInfo(userId);
            // Add behavior analysis logic here
            return true;
        }
        catch (...) {
            return false;
        }
    }

    bool analyzeRelationships(const std::string& userId) {
        try {
            // Analyze user relationships
            json userGuilds = checkDiscordGuilds(userId);
            // Add relationship analysis logic here
            return true;
        }
        catch (...) {
            return false;
        }
    }

    bool analyzeLinkedAccounts(const std::string& userId) {
        try {
            // Check for linked accounts
            json userInfo = getDiscordUserInfo(userId);
            // Add linked accounts analysis logic here
            return true;
        }
        catch (...) {
            return false;
        }
    }

    bool analyzeActivity(const std::string& userId) {
        try {
            // Analyze user activity patterns
            json userInfo = getDiscordUserInfo(userId);
            // Add activity analysis logic here
            return true;
        }
        catch (...) {
            return false;
        }
    }

    bool analyzePresence(const std::string& userId) {
        try {
            // Analyze user presence patterns
            json userInfo = getDiscordUserInfo(userId);
            // Add presence analysis logic here
            return true;
        }
        catch (...) {
            return false;
        }
    }

    std::map<std::string, std::string> getCommonServers(const std::string& userId) {
        std::map<std::string, std::string> commonServers;
        try {
            json userGuilds = checkDiscordGuilds(userId);
            // Add common servers analysis logic here
            return commonServers;
        }
        catch (...) {
            return commonServers;
        }
    }

    std::map<std::string, int> getActivityStats(const std::string& userId) {
        std::map<std::string, int> stats;
        try {
            json userInfo = getDiscordUserInfo(userId);
            // Add activity stats analysis logic here
            return stats;
        }
        catch (...) {
            return stats;
        }
    }

    std::vector<std::string> getLinkedPlatforms(const std::string& userId) {
        std::vector<std::string> platforms;
        try {
            json userInfo = getDiscordUserInfo(userId);
            // Add linked platforms analysis logic here
            return platforms;
        }
        catch (...) {
            return platforms;
        }
    }
};
