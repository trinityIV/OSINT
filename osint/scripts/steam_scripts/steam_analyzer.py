import requests
import json
from typing import Dict, List, Optional

class SteamAnalyzer:
    def __init__(self, api_key: str):
        self.api_key = api_key
        self.base_url = "https://api.steampowered.com"

    def get_player_info(self, steam_id: str) -> Dict:
        """Récupère les informations d'un joueur Steam"""
        url = f"{self.base_url}/ISteamUser/GetPlayerSummaries/v2/"
        params = {
            "key": self.api_key,
            "steamids": steam_id
        }
        response = requests.get(url, params=params)
        return response.json()

    def scan_inventory(self, steam_id: str, app_id: int = 730) -> Dict:
        """Scanne l'inventaire d'un joueur pour un jeu spécifique"""
        url = f"https://steamcommunity.com/inventory/{steam_id}/{app_id}/2"
        response = requests.get(url)
        return response.json()

    def find_running_games(self, steam_id: str) -> List[Dict]:
        """Trouve les jeux en cours d'exécution pour un joueur"""
        url = f"{self.base_url}/IPlayerService/GetRecentlyPlayedGames/v1/"
        params = {
            "key": self.api_key,
            "steamid": steam_id,
            "count": 1
        }
        response = requests.get(url, params=params)
        return response.json()

    def get_player_bans(self, steam_id: str) -> Dict:
        """Vérifie les bans d'un joueur"""
        url = f"{self.base_url}/ISteamUser/GetPlayerBans/v1/"
        params = {
            "key": self.api_key,
            "steamids": steam_id
        }
        response = requests.get(url, params=params)
        return response.json()

if __name__ == "__main__":
    # Example usage
    with open("../../config.json") as f:
        config = json.load(f)
    
    analyzer = SteamAnalyzer(config["steam"]["api_key"])
    # Test avec un Steam ID
    steam_id = "76561198xxxxxxxxx"  # Remplacer par un vrai Steam ID
    print(analyzer.get_player_info(steam_id))
