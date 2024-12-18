-- Steam Analyzer Script
local json = require("json")
local http = require("http")

-- Configuration
local api_key = "" -- À remplir depuis config.json

-- Fonctions d'analyse Steam
function get_player_info(steam_id)
    local url = string.format("http://api.steampowered.com/ISteamUser/GetPlayerSummaries/v2/?key=%s&steamids=%s", api_key, steam_id)
    local response = http.get(url)
    return json.decode(response)
end

function scan_inventory(steam_id)
    local url = string.format("http://steamcommunity.com/inventory/%s/730/2", steam_id)
    local response = http.get(url)
    return json.decode(response)
end

function find_friends(steam_id)
    local url = string.format("http://api.steampowered.com/ISteamUser/GetFriendList/v1/?key=%s&steamid=%s", api_key, steam_id)
    local response = http.get(url)
    return json.decode(response)
end

-- Export des fonctions
return {
    get_player_info = get_player_info,
    scan_inventory = scan_inventory,
    find_friends = find_friends
}
