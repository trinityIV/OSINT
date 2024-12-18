-- Discord Analyzer Script
local json = require("json")
local http = require("http")

-- Configuration
local token = "" -- À remplir depuis config.json

-- Fonctions d'analyse Discord
function get_user_info(user_id)
    local headers = {
        ["Authorization"] = "Bot " .. token
    }
    local url = string.format("https://discord.com/api/v9/users/%s", user_id)
    local response = http.get(url, headers)
    return json.decode(response)
end

function scan_messages(channel_id, limit)
    local headers = {
        ["Authorization"] = "Bot " .. token
    }
    local url = string.format("https://discord.com/api/v9/channels/%s/messages?limit=%d", channel_id, limit)
    local response = http.get(url, headers)
    return json.decode(response)
end

function get_server_info(guild_id)
    local headers = {
        ["Authorization"] = "Bot " .. token
    }
    local url = string.format("https://discord.com/api/v9/guilds/%s", guild_id)
    local response = http.get(url, headers)
    return json.decode(response)
end

-- Export des fonctions
return {
    get_user_info = get_user_info,
    scan_messages = scan_messages,
    get_server_info = get_server_info
}
