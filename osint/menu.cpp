#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <string>
#include <vector>
#include "discord_analyzer.hpp"
#include "steam_analyzer.hpp"
#include "member_analyzer.hpp"
#include "channel_monitor.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include "ascii_animation.hpp"

using json = nlohmann::json;

// Structures pour les menus
struct MenuItem {
    std::string name;
    std::string description;
    void (*function)();
};

struct Menu {
    std::string title;
    std::vector<MenuItem> items;
};

// Variables globales pour les analyseurs
std::unique_ptr<DiscordAnalyzer> discordAnalyzer;
std::unique_ptr<SteamAnalyzer> steamAnalyzer;
std::unique_ptr<MemberAnalyzer> memberAnalyzer;
std::unique_ptr<ChannelMonitor> channelMonitor;
std::unique_ptr<AsciiAnimation> asciiAnimation;

// Déclarations des fonctions pour chaque outil
void runMemoryScanner();
void runNetworkScanner();
void runProcessAnalyzer();
void runWebScraper();
void runSocialMediaScanner();
void runEmailOSINT();
void runDomainAnalyzer();
void runVulnerabilityScanner();
void runMetadataAnalyzer();
void runDarkwebScanner();
void runDiscordAnalyzer();
void runSteamAnalyzer();
void runMemberAnalyzer();
void runChannelMonitor();

// Fonction pour charger la configuration
void loadConfig() {
    try {
        std::ifstream configFile("config.json");
        json config = json::parse(configFile);
        
        std::string discordToken = config["discord"]["token"];
        std::string steamApiKey = config["steam"]["api_key"];
        
        discordAnalyzer = std::make_unique<DiscordAnalyzer>(discordToken);
        steamAnalyzer = std::make_unique<SteamAnalyzer>(steamApiKey);
        memberAnalyzer = std::make_unique<MemberAnalyzer>(discordToken, steamApiKey);
        // Le channelMonitor sera créé plus tard quand l'utilisateur choisira un canal à surveiller
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur lors du chargement de la configuration: " << e.what() << std::endl;
    }
}

void analyzeDiscordUser() {
    system("cls");
    std::string userId;
    std::cout << "Analyse d'utilisateur Discord\n";
    std::cout << "Entrez l'ID de l'utilisateur: ";
    std::cin >> userId;

    try {
        json userInfo = discordAnalyzer->getUserInfo(userId);
        
        std::cout << "\nInformations trouvées:\n";
        std::cout << "Nom d'utilisateur: " << userInfo["username"].get<std::string>() << "\n";
        std::cout << "Discriminateur: " << userInfo["discriminator"].get<std::string>() << "\n";
        std::cout << "ID: " << userInfo["id"].get<std::string>() << "\n";
        if (userInfo.contains("avatar")) {
            std::cout << "Avatar: https://cdn.discordapp.com/avatars/" << userId << "/" 
                     << userInfo["avatar"].get<std::string>() << ".png\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur lors de l'analyse: " << e.what() << std::endl;
    }
}

void analyzeSteamProfile() {
    system("cls");
    std::string steamId;
    std::cout << "Analyse de profil Steam\n";
    std::cout << "Entrez le Steam ID: ";
    std::cin >> steamId;

    try {
        json playerInfo = steamAnalyzer->getPlayerSummaries(steamId);
        json playerBans = steamAnalyzer->getPlayerBans(steamId);
        json ownedGames = steamAnalyzer->getOwnedGames(steamId);
        
        auto& player = playerInfo["response"]["players"][0];
        
        std::cout << "\nInformations trouvées:\n";
        std::cout << "Nom: " << player["personaname"].get<std::string>() << "\n";
        std::cout << "Profil: " << player["profileurl"].get<std::string>() << "\n";
        std::cout << "Statut: " << (player["personastate"].get<int>() == 1 ? "En ligne" : "Hors ligne") << "\n";
        
        if (!playerBans["players"].empty()) {
            auto& bans = playerBans["players"][0];
            std::cout << "VAC Bans: " << bans["NumberOfVACBans"].get<int>() << "\n";
            std::cout << "Game Bans: " << bans["NumberOfGameBans"].get<int>() << "\n";
        }
        
        if (ownedGames.contains("response")) {
            std::cout << "Nombre de jeux: " << ownedGames["response"]["game_count"].get<int>() << "\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur lors de l'analyse: " << e.what() << std::endl;
    }
}

void runMemoryScanner() {
    system("cls");
    std::cout << "Lancement du Memory Scanner...\n";
    system("cd memory_scanner && memory_scanner.exe");
}

void runNetworkScanner() {
    system("cls");
    std::cout << "Scanner Réseau\n";
    std::cout << "1. Scan des ports\n";
    std::cout << "2. Détection des appareils\n";
    std::cout << "3. Analyse du trafic\n";
    // TODO: Implémenter la logique
}

void runProcessAnalyzer() {
    system("cls");
    std::cout << "Analyseur de Processus\n";
    std::cout << "Liste des processus en cours...\n";
    // TODO: Implémenter la logique
}

void runWebScraper() {
    system("cls");
    std::cout << "Web Scraper\n";
    std::cout << "1. Extraction d'informations\n";
    std::cout << "2. Analyse de site web\n";
    // TODO: Implémenter la logique
}

void runSocialMediaScanner() {
    system("cls");
    std::cout << "Scanner de Médias Sociaux\n";
    std::cout << "1. Recherche de profils\n";
    std::cout << "2. Analyse de contenu\n";
    // TODO: Implémenter la logique
}

void runEmailOSINT() {
    system("cls");
    std::cout << "Email OSINT\n";
    std::cout << "1. Vérification d'adresse email\n";
    std::cout << "2. Recherche de fuites de données\n";
    // TODO: Implémenter la logique
}

void runDomainAnalyzer() {
    system("cls");
    std::cout << "Analyseur de Domaine\n";
    std::cout << "1. Whois\n";
    std::cout << "2. DNS Lookup\n";
    std::cout << "3. Historique du domaine\n";
    // TODO: Implémenter la logique
}

void runVulnerabilityScanner() {
    system("cls");
    std::cout << "Scanner de Vulnérabilités\n";
    std::cout << "1. Scan de ports ouverts\n";
    std::cout << "2. Détection de services\n";
    std::cout << "3. Analyse de vulnérabilités connues\n";
    // TODO: Implémenter la logique
}

void runMetadataAnalyzer() {
    system("cls");
    std::cout << "Analyseur de Métadonnées\n";
    std::cout << "1. Analyse de documents\n";
    std::cout << "2. Analyse d'images\n";
    // TODO: Implémenter la logique
}

void runDarkwebScanner() {
    system("cls");
    std::cout << "Scanner Dark Web\n";
    std::cout << "1. Recherche d'informations\n";
    std::cout << "2. Analyse de marchés\n";
    // TODO: Implémenter la logique
}

void runDiscordAnalyzer() {
    while (true) {
        system("cls");
        std::cout << "=== Discord Analyzer ===\n\n";
        std::cout << "1. Analyser un utilisateur\n";
        std::cout << "2. Analyser un serveur\n";
        std::cout << "3. Rechercher des messages\n";
        std::cout << "4. Scanner les webhooks\n";
        std::cout << "0. Retour au menu principal\n\n";

        int choice;
        std::cout << "Choix: ";
        std::cin >> choice;

        switch (choice) {
            case 0:
                return;
            case 1:
                analyzeDiscordUser();
                break;
            case 2:
                // TODO: Implémenter l'analyse de serveur
                break;
            case 3:
                std::cout << "Recherche de messages...\n";
                break;
            case 4:
                std::cout << "Scan des webhooks...\n";
                break;
            default:
                std::cout << "Option invalide!\n";
        }
        std::cout << "\nAppuyez sur une touche pour continuer...";
        std::cin.ignore();
        std::cin.get();
    }
}

void runSteamAnalyzer() {
    while (true) {
        system("cls");
        std::cout << "=== Steam Analyzer ===\n\n";
        std::cout << "1. Analyser un profil\n";
        std::cout << "2. Analyser un jeu\n";
        std::cout << "3. Scanner les inventaires\n";
        std::cout << "4. Rechercher des groupes\n";
        std::cout << "0. Retour au menu principal\n\n";

        int choice;
        std::cout << "Choix: ";
        std::cin >> choice;

        switch (choice) {
            case 0:
                return;
            case 1:
                analyzeSteamProfile();
                break;
            case 2:
                // TODO: Implémenter l'analyse de jeu
                break;
            case 3:
                std::cout << "Scan des inventaires...\n";
                break;
            case 4:
                std::cout << "Recherche de groupes...\n";
                break;
            default:
                std::cout << "Option invalide!\n";
        }
        std::cout << "\nAppuyez sur une touche pour continuer...";
        std::cin.ignore();
        std::cin.get();
    }
}

void runMemberAnalyzer() {
    while (true) {
        system("cls");
        std::cout << "=== Member Analyzer ===\n\n";
        std::cout << "1. Analyser un serveur Discord\n";
        std::cout << "2. Analyser un groupe Steam\n";
        std::cout << "3. Analyse comportementale (Discord)\n";
        std::cout << "4. Analyse comportementale (Steam)\n";
        std::cout << "5. Rechercher des comptes liés (Discord)\n";
        std::cout << "6. Rechercher des comptes liés (Steam)\n";
        std::cout << "7. Analyse de localisation (Discord)\n";
        std::cout << "8. Analyse de localisation (Steam)\n";
        std::cout << "9. Analyse des relations (Discord)\n";
        std::cout << "10. Analyse des relations (Steam)\n";
        std::cout << "0. Retour au menu principal\n\n";

        int choice;
        std::cout << "Choix: ";
        std::cin >> choice;

        std::string id;
        json result;

        switch (choice) {
            case 0:
                return;
            case 1: {
                std::string serverId;
                std::cout << "Enter Discord server ID: ";
                std::cin >> serverId;
                result = memberAnalyzer->checkDiscordGuilds(serverId);
                std::cout << "\nResultats de l'analyse:\n" << result.dump(2) << std::endl;
                break;
            }
            case 2: {
                std::string steamId;
                std::cout << "Enter Steam group ID: ";
                std::cin >> steamId;
                result = memberAnalyzer->checkSteamGroups(steamId);
                std::cout << "\nResultats de l'analyse:\n" << result.dump(2) << std::endl;
                break;
            }
            case 3: {
                std::string userId;
                std::cout << "Enter user ID: ";
                std::cin >> userId;
                bool result = memberAnalyzer->analyzeBehavior(userId);
                std::cout << "Behavior analysis " << (result ? "completed" : "failed") << std::endl;
                break;
            }
            case 4: {
                std::string userId;
                std::cout << "Enter user ID: ";
                std::cin >> userId;
                bool result = memberAnalyzer->analyzeBehavior(userId);
                std::cout << "Behavior analysis " << (result ? "completed" : "failed") << std::endl;
                break;
            }
            case 5: {
                std::string userId;
                std::cout << "Enter user ID: ";
                std::cin >> userId;
                std::vector<std::string> platforms = memberAnalyzer->getLinkedPlatforms(userId);
                std::cout << "Found " << platforms.size() << " linked platforms" << std::endl;
                for (const auto& platform : platforms) {
                    std::cout << "- " << platform << std::endl;
                }
                break;
            }
            case 6: {
                std::string userId;
                std::cout << "Enter user ID: ";
                std::cin >> userId;
                std::vector<std::string> platforms = memberAnalyzer->getLinkedPlatforms(userId);
                std::cout << "Found " << platforms.size() << " linked platforms" << std::endl;
                for (const auto& platform : platforms) {
                    std::cout << "- " << platform << std::endl;
                }
                break;
            }
            case 7: {
                std::string userId;
                std::cout << "Enter user ID: ";
                std::cin >> userId;
                bool result = memberAnalyzer->analyzeActivity(userId);
                std::cout << "Activity analysis " << (result ? "completed" : "failed") << std::endl;
                break;
            }
            case 8: {
                std::string userId;
                std::cout << "Enter user ID: ";
                std::cin >> userId;
                bool result = memberAnalyzer->analyzeActivity(userId);
                std::cout << "Activity analysis " << (result ? "completed" : "failed") << std::endl;
                break;
            }
            case 9: {
                std::string userId;
                std::cout << "Enter user ID: ";
                std::cin >> userId;
                bool result = memberAnalyzer->analyzeRelationships(userId);
                std::cout << "Relationship analysis " << (result ? "completed" : "failed") << std::endl;
                break;
            }
            case 10: {
                std::string userId;
                std::cout << "Enter user ID: ";
                std::cin >> userId;
                bool result = memberAnalyzer->analyzeRelationships(userId);
                std::cout << "Relationship analysis " << (result ? "completed" : "failed") << std::endl;
                break;
            }
            default:
                std::cout << "Option invalide!\n";
                continue;
        }
        
        std::cout << "\nAppuyez sur une touche pour continuer...";
        std::cin.ignore();
        std::cin.get();
    }
}

void runChannelMonitor() {
    static std::string currentChannelId;
    
    while (true) {
        system("cls");
        std::cout << "=== Surveillance de Canal Discord ===\n\n";
        std::cout << "1. Démarrer la surveillance\n";
        std::cout << "2. Arrêter la surveillance\n";
        std::cout << "3. Voir les membres actuels\n";
        std::cout << "4. Détails d'un membre\n";
        std::cout << "0. Retour au menu principal\n\n";

        if (!currentChannelId.empty()) {
            std::cout << "Canal actuellement surveillé: " << currentChannelId << "\n\n";
        }

        int choice;
        std::cout << "Choix: ";
        std::cin >> choice;

        std::string id;
        json result;

        switch (choice) {
            case 0:
                if(channelMonitor) channelMonitor->stop();
                return;
            case 1:
                std::cout << "Enter channel ID: ";
                std::cin >> currentChannelId;
                channelMonitor = std::make_unique<ChannelMonitor>(discordAnalyzer->getDiscordToken(), currentChannelId);
                channelMonitor->start();
                std::cout << "Channel monitoring started." << std::endl;
                break;
            case 2:
                if(channelMonitor) channelMonitor->stop();
                currentChannelId.clear();
                std::cout << "Channel monitoring stopped." << std::endl;
                break;
            case 3: {
                if(channelMonitor) channelMonitor->stop();
                std::cout << "Channel monitoring stopped." << std::endl;
                break;
            }
            case 4: {
                std::cout << "Current channel members:" << std::endl;
                if(channelMonitor) {
                    json members = channelMonitor->getCurrentMembers();
                    std::cout << "\nResultats de l'analyse:\n" << members.dump(2) << std::endl;
                }
                break;
            }
            case 21: {
                if(channelMonitor) channelMonitor->stop();
                std::cout << "Channel monitoring stopped." << std::endl;
                break;
            }
            case 22: {
                std::string channelId;
                std::cout << "Enter channel ID: ";
                std::cin >> channelId;
                channelMonitor = std::make_unique<ChannelMonitor>(discordAnalyzer->getDiscordToken(), channelId);
                channelMonitor->start();
                std::cout << "Channel monitoring started." << std::endl;
                break;
            }
            case 23: {
                if(channelMonitor) channelMonitor->stop();
                std::cout << "Channel monitoring stopped." << std::endl;
                break;
            }
            case 24: {
                std::cout << "Current channel members:" << std::endl;
                if(channelMonitor) {
                    json members = channelMonitor->getCurrentMembers();
                    std::cout << "\nResultats de l'analyse:\n" << members.dump(2) << std::endl;
                }
                break;
            }
            case 25: {
                std::string memberId;
                std::cout << "Enter member ID: ";
                std::cin >> memberId;
                json memberInfo = memberAnalyzer->getDiscordUserInfo(memberId);
                std::cout << "\nResultats de l'analyse:\n" << memberInfo.dump(2) << std::endl;
                break;
            }
            default:
                std::cout << "Option invalide!\n";
                continue;
        }
        
        std::cout << "\nAppuyez sur une touche pour continuer...";
        std::cin.ignore();
        std::cin.get();
    }
}

// Fonction pour afficher le menu
void displayMenu(const Menu& menu) {
    system("cls");
    
    asciiAnimation->displayStaticLogo();
    
    std::cout << "\n=== " << menu.title << " ===\n\n";
    
    for (size_t i = 0; i < menu.items.size(); ++i) {
        std::cout << i + 1 << ". " << menu.items[i].name;
        std::cout << " - " << menu.items[i].description << "\n";
    }
    std::cout << "0. Quitter\n\n";
    
    int choice;
    std::cout << "Choix: ";
    std::cin >> choice;
    
    if (choice == 0) {
        std::cout << "Au revoir!\n";
        exit(0);
    }
    else if (choice > 0 && choice <= static_cast<int>(menu.items.size())) {
        asciiAnimation->start();
        menu.items[choice - 1].function();
        asciiAnimation->stop();
    }
    else {
        std::cout << "Option invalide!\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    loadConfig();
    
    asciiAnimation = std::make_unique<AsciiAnimation>();
    
    asciiAnimation->start();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    asciiAnimation->stop();

    // Configuration du menu principal
    Menu mainMenu;
    mainMenu.title = "OSINT Security Tool Suite";
    mainMenu.items = {
        {"Memory Scanner", "Scanner et analyser la mémoire des processus", runMemoryScanner},
        {"Network Scanner", "Scanner et analyser le réseau", runNetworkScanner},
        {"Process Analyzer", "Analyser les processus en cours d'exécution", runProcessAnalyzer},
        {"Discord Analyzer", "Analyser les données Discord", runDiscordAnalyzer},
        {"Steam Analyzer", "Analyser les données Steam", runSteamAnalyzer},
        {"Member Analyzer", "Analyser les membres Discord et Steam", runMemberAnalyzer},
        {"Channel Monitor", "Surveiller les membres d'un canal Discord", runChannelMonitor}
    };

    while (true) {
        displayMenu(mainMenu);
    }

    return 0;
}
