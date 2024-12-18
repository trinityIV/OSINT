#pragma once
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <windows.h>
#include <shellapi.h>
#include "http_client.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ChannelMonitor {
private:
    HttpClient client;
    std::string token;
    std::string channelId;
    std::vector<std::string> currentMembers;
    std::mutex membersMutex;
    std::condition_variable cv;
    bool running;
    std::thread monitorThread;

    void showNotification(const std::string& title, const std::string& message) {
        // Get the path to the executable
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);

        // Create notification
        NOTIFYICONDATAA nid = {};
        nid.cbSize = sizeof(NOTIFYICONDATAA);
        nid.hWnd = NULL;
        nid.uID = 1;
        nid.uFlags = NIF_INFO | NIF_ICON;
        nid.dwInfoFlags = NIIF_INFO;
        strcpy_s(nid.szInfoTitle, sizeof(nid.szInfoTitle), title.c_str());
        strcpy_s(nid.szInfo, sizeof(nid.szInfo), message.c_str());
        nid.hIcon = ExtractIconA(GetModuleHandleA(NULL), exePath, 0);

        Shell_NotifyIconA(NIM_ADD, &nid);
        Sleep(5000);  // Show notification for 5 seconds
        Shell_NotifyIconA(NIM_DELETE, &nid);
        if (nid.hIcon) DestroyIcon(nid.hIcon);
    }

public:
    ChannelMonitor(const std::string& token, const std::string& channelId)
        : token(token), channelId(channelId), running(false) {}

    ~ChannelMonitor() {
        stop();
    }

    json getCurrentMembers() {
        std::string url = "https://discord.com/api/v10/channels/" + channelId + "/messages";
        return client.get(url);
    }

    void start() {
        if (running) return;
        running = true;
        monitorThread = std::thread([this]() {
            while (running) {
                try {
                    json members = getCurrentMembers();
                    std::vector<std::string> newMembers;

                    for (const auto& message : members) {
                        newMembers.push_back(message["author"]["username"].get<std::string>());
                    }

                    {
                        std::lock_guard<std::mutex> lock(membersMutex);
                        if (newMembers != currentMembers) {
                            // Find new members
                            for (const auto& member : newMembers) {
                                if (std::find(currentMembers.begin(), currentMembers.end(), member) == currentMembers.end()) {
                                    showNotification("New Member", member + " joined the channel");
                                }
                            }

                            // Find members who left
                            for (const auto& member : currentMembers) {
                                if (std::find(newMembers.begin(), newMembers.end(), member) == newMembers.end()) {
                                    showNotification("Member Left", member + " left the channel");
                                }
                            }

                            currentMembers = newMembers;
                        }
                    }
                }
                catch (...) {
                    // Handle error or log it
                }

                std::this_thread::sleep_for(std::chrono::seconds(30));
            }
        });
    }

    void stop() {
        if (!running) return;
        running = false;
        cv.notify_all();
        if (monitorThread.joinable()) {
            monitorThread.join();
        }
    }

    std::vector<std::string> getMembers() {
        std::lock_guard<std::mutex> lock(membersMutex);
        return currentMembers;
    }
};
