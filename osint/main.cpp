#include <iostream>
#include <windows.h>
#include <vector>
#include <winsock2.h>
#include <memory>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include "ascii_animation.hpp"
#include "discord_analyzer.hpp"
#include "steam_analyzer.hpp"
#include "member_analyzer.hpp"
#include "channel_monitor.hpp"
#include "http_client.hpp"

#pragma comment(lib, "ws2_32.lib")

using json = nlohmann::json;

// Déclaration des variables globales
extern std::unique_ptr<DiscordAnalyzer> discordAnalyzer;
extern std::unique_ptr<SteamAnalyzer> steamAnalyzer;
extern std::unique_ptr<MemberAnalyzer> memberAnalyzer;
extern std::unique_ptr<ChannelMonitor> channelMonitor;
extern std::unique_ptr<AsciiAnimation> asciiAnimation;

void printProcessList() {
    // This function will list all processes running on the system.
    // For now, it will just print a placeholder message.
    std::cout << "Listing all processes..." << std::endl;
}

// Function to scan memory of a process
void scanMemory(DWORD processID) {
    // Placeholder for memory scanning logic
    std::cout << "Scanning memory of process ID: " << processID << std::endl;
}

// Function to search for a pattern in memory
void searchPattern(DWORD processID, const std::vector<BYTE>& pattern) {
    // Placeholder for pattern searching logic
    std::cout << "Searching for pattern in process ID: " << processID << std::endl;
}

// Example usage of the new functions
void exampleUsage() {
    DWORD exampleProcessID = 1234; // Example process ID
    std::vector<BYTE> examplePattern = {0x90, 0x90, 0x90}; // Example pattern
    scanMemory(exampleProcessID);
    searchPattern(exampleProcessID, examplePattern);
}

void initializeWinsock() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        exit(1);
    }
}

void communicateWithServer() {
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in clientService;

    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_port = htons(3000);

    if (connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect." << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    // Send and receive data here
    const char* sendbuf = "this is a test";
    send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);

    closesocket(ConnectSocket);
    WSACleanup();
}

// Note: loadConfig() et main() sont maintenant dans menu.cpp
