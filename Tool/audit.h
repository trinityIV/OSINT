#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iphlpapi.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <functional>

class ApplicationAuditor {
public:
    struct NetworkConnection {
        std::string localAddress;
        std::string remoteAddress;
        uint16_t localPort;
        uint16_t remotePort;
        std::string protocol;
        std::string state;
        double bandwidth;
        std::string processName;
        DWORD pid;
    };

    struct ProcessInfo {
        DWORD pid;
        std::string name;
        std::string path;
        std::vector<std::string> modules;
        std::map<std::string, double> resourceUsage;
        double cpuUsage;
        SIZE_T memoryUsage;
        std::vector<NetworkConnection> connections;
    };

    ApplicationAuditor();
    ~ApplicationAuditor();

    // Méthodes principales
    ProcessInfo analyzeProcess(const std::string& processName);
    bool startContinuousMonitoring(DWORD pid, const std::string& logPath);
    void stopMonitoring();
    void generateAuditReport(const ProcessInfo& info, const std::string& outputPath);

    // Nouvelles méthodes de filtrage
    std::vector<ProcessInfo> filterProcessesByResource(double threshold);
    std::vector<NetworkConnection> filterConnectionsByState(const std::string& state);
    std::vector<ProcessInfo> filterProcessesByMemory(SIZE_T threshold);
    std::vector<ProcessInfo> filterProcessesByCPU(double threshold);
    
    // Méthodes de recherche avancée
    std::vector<ProcessInfo> findSuspiciousProcesses();
    std::vector<NetworkConnection> findSuspiciousConnections();
    
    // Gestion des erreurs
    std::string getLastError() const;
    void clearError();

private:
    bool monitoring;
    std::string lastError;
    std::unique_ptr<std::thread> monitorThread;
    std::vector<ProcessInfo> processHistory;
    
    // Méthodes privées
    double calculateCPUUsage(DWORD pid);
    SIZE_T getProcessMemoryUsage(DWORD pid);
    std::vector<NetworkConnection> getProcessConnections(DWORD pid);
    void updateProcessHistory(const ProcessInfo& info);
    bool isProcessSuspicious(const ProcessInfo& process);
    bool isConnectionSuspicious(const NetworkConnection& connection);
};