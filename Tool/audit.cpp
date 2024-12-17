#include "../Tool/audit.h"
#include <pdh.h>
#include <sstream>
#include <chrono>
#include <thread>

ApplicationAuditor::ApplicationAuditor() : 
    isMonitoring(false), 
    monitoringThread(NULL), 
    targetPid(0) {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    numProcessors = sysInfo.dwNumberOfProcessors;

    // Initialize CPU tracking
    FILETIME ftime, fsys, fuser;
    GetSystemTimeAsFileTime(&ftime);
    memcpy(&lastCPU, &ftime, sizeof(FILETIME));
    GetProcessTimes(GetCurrentProcess(), &ftime, &ftime, &fsys, &fuser);
    memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
    memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
}

ApplicationAuditor::~ApplicationAuditor() {
    stopMonitoring();
}

ProcessInfo ApplicationAuditor::analyzeProcess(const std::string& processName) {
    ProcessInfo info;
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(processEntry);
        
        if (Process32FirstW(snapshot, &processEntry)) {
            do {
                std::wstring wProcessName = processEntry.szExeFile;
                std::string currentProcessName(wProcessName.begin(), wProcessName.end());
                
                if (currentProcessName == processName) {
                    info.pid = processEntry.th32ProcessID;
                    info.name = processName;
                    
                    // Get process path
                    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, info.pid);
                    if (processHandle != NULL) {
                        wchar_t pathBuffer[MAX_PATH];
                        if (GetModuleFileNameExW(processHandle, NULL, pathBuffer, MAX_PATH)) {
                            std::wstring wPath(pathBuffer);
                            info.path = std::string(wPath.begin(), wPath.end());
                        }
                        CloseHandle(processHandle);
                    }
                    break;
                }
            } while (Process32NextW(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }

    // Gather additional information
    info.resourceUsage = getResourceUsage(info.pid);
    info.modules = listLoadedModules(info.pid);
    info.activeConnections = monitorNetworkActivity(info.pid);
    info.memoryUsage = getProcessMemoryUsage(info.pid);
    info.cpuUsage = calculateCPUUsage(info.pid);
    info.openHandles = getProcessOpenHandles(info.pid);

    return info;
}

std::vector<NetworkConnection> ApplicationAuditor::monitorNetworkActivity(DWORD pid) {
    std::vector<NetworkConnection> connections;
    
    // TCP connections
    MIB_TCPTABLE_OWNER_PID* pTcpTable = nullptr;
    DWORD dwSize = 0;
    
    if (GetExtendedTcpTable(nullptr, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == ERROR_INSUFFICIENT_BUFFER) {
        pTcpTable = (MIB_TCPTABLE_OWNER_PID*)new char[dwSize];
        if (GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == NO_ERROR) {
            for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
                if (pTcpTable->table[i].dwOwningPid == pid) {
                    NetworkConnection conn;
                    conn.localPort = ntohs((u_short)pTcpTable->table[i].dwLocalPort);
                    conn.remotePort = ntohs((u_short)pTcpTable->table[i].dwRemotePort);
                    conn.protocol = "TCP";
                    conn.state = getTcpState(pTcpTable->table[i].dwState);
                    conn.pid = pid;
                    
                    // Convert IP addresses
                    in_addr localAddr, remoteAddr;
                    localAddr.s_addr = pTcpTable->table[i].dwLocalAddr;
                    remoteAddr.s_addr = pTcpTable->table[i].dwRemoteAddr;
                    conn.localAddress = inet_ntoa(localAddr);
                    conn.remoteAddress = inet_ntoa(remoteAddr);
                    
                    connections.push_back(conn);
                }
            }
        }
        delete[] pTcpTable;
    }
    
    return connections;
}

std::string ApplicationAuditor::getTcpState(DWORD state) {
    switch (state) {
        case MIB_TCP_STATE_CLOSED: return "CLOSED";
        case MIB_TCP_STATE_LISTEN: return "LISTENING";
        case MIB_TCP_STATE_ESTAB: return "ESTABLISHED";
        case MIB_TCP_STATE_SYN_SENT: return "SYN_SENT";
        case MIB_TCP_STATE_SYN_RCVD: return "SYN_RECEIVED";
        case MIB_TCP_STATE_FIN_WAIT1: return "FIN_WAIT1";
        case MIB_TCP_STATE_FIN_WAIT2: return "FIN_WAIT2";
        case MIB_TCP_STATE_CLOSE_WAIT: return "CLOSE_WAIT";
        case MIB_TCP_STATE_CLOSING: return "CLOSING";
        case MIB_TCP_STATE_LAST_ACK: return "LAST_ACK";
        case MIB_TCP_STATE_TIME_WAIT: return "TIME_WAIT";
        case MIB_TCP_STATE_DELETE_TCB: return "DELETE_TCB";
        default: return "UNKNOWN";
    }
}

void ApplicationAuditor::startContinuousMonitoring(DWORD pid, const std::string& logPath) {
    if (!isMonitoring) {
        targetPid = pid;
        logFilePath = logPath;
        isMonitoring = true;
        monitoringThread = CreateThread(NULL, 0, MonitoringThreadProc, this, 0, NULL);
    }
}

void ApplicationAuditor::stopMonitoring() {
    if (isMonitoring) {
        isMonitoring = false;
        if (monitoringThread != NULL) {
            WaitForSingleObject(monitoringThread, INFINITE);
            CloseHandle(monitoringThread);
            monitoringThread = NULL;
        }
    }
}

DWORD WINAPI ApplicationAuditor::MonitoringThreadProc(LPVOID lpParameter) {
    ApplicationAuditor* auditor = static_cast<ApplicationAuditor*>(lpParameter);
    auditor->monitoringLoop();
    return 0;
}

void ApplicationAuditor::monitoringLoop() {
    std::ofstream logFile(logFilePath, std::ios::app);
    
    while (isMonitoring) {
        ProcessInfo info = analyzeProcess(std::to_string(targetPid));
        
        // Log current state
        auto now = std::chrono::system_clock::now();
        auto nowTime = std::chrono::system_clock::to_time_t(now);
        
        logFile << "=== " << std::ctime(&nowTime);
        logFile << "CPU Usage: " << info.cpuUsage << "%\n";
        logFile << "Memory Usage: " << (info.memoryUsage / 1024.0 / 1024.0) << " MB\n";
        
        // Log network connections
        logFile << "Active Connections:\n";
        for (const auto& conn : info.activeConnections) {
            logFile << conn.localAddress << ":" << conn.localPort << " -> "
                   << conn.remoteAddress << ":" << conn.remotePort
                   << " (" << conn.protocol << " - " << conn.state << ")\n";
        }
        logFile << "\n";
        logFile.flush();
        
        // Wait before next sample
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

double ApplicationAuditor::calculateCPUUsage(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (process == NULL) return -1.0;

    FILETIME ftime, fsys, fuser;
    FILETIME now, sys, user;
    double percent;

    GetSystemTimeAsFileTime(&now);
    if (!GetProcessTimes(process, &ftime, &ftime, &sys, &user)) {
        CloseHandle(process);
        return -1.0;
    }
    
    ULARGE_INTEGER ul_sys, ul_user, ul_now;
    memcpy(&ul_sys, &sys, sizeof(FILETIME));
    memcpy(&ul_user, &user, sizeof(FILETIME));
    memcpy(&ul_now, &now, sizeof(FILETIME));

    percent = (ul_sys.QuadPart - lastSysCPU.QuadPart) +
              (ul_user.QuadPart - lastUserCPU.QuadPart);
    percent /= (ul_now.QuadPart - lastCPU.QuadPart);
    percent /= numProcessors;
    percent *= 100;

    lastCPU = ul_now;
    lastUserCPU = ul_user;
    lastSysCPU = ul_sys;

    CloseHandle(process);
    return percent;
}

SIZE_T ApplicationAuditor::getProcessMemoryUsage(DWORD pid) {
    PROCESS_MEMORY_COUNTERS pmc;
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (process == NULL) return 0;
    
    SIZE_T memoryUsage = 0;
    if (GetProcessMemoryInfo(process, &pmc, sizeof(pmc))) {
        memoryUsage = pmc.WorkingSetSize;
    }
    
    CloseHandle(process);
    return memoryUsage;
}

void ApplicationAuditor::generateAuditReport(const ProcessInfo& info, const std::string& outputPath) {
    std::ofstream report(outputPath);
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    
    report << "Application Audit Report\n";
    report << "=======================\n";
    report << "Generated: " << std::ctime(&nowTime);
    report << "\nProcess Information:\n";
    report << "-------------------\n";
    report << "Name: " << info.name << "\n";
    report << "PID: " << info.pid << "\n";
    report << "Path: " << info.path << "\n";
    report << "CPU Usage: " << info.cpuUsage << "%\n";
    report << "Memory Usage: " << (info.memoryUsage / 1024.0 / 1024.0) << " MB\n";
    
    report << "\nLoaded Modules:\n";
    report << "--------------\n";
    for (const auto& module : info.modules) {
        report << module << "\n";
    }
    
    report << "\nNetwork Connections:\n";
    report << "------------------\n";
    for (const auto& conn : info.activeConnections) {
        report << conn.localAddress << ":" << conn.localPort << " -> "
               << conn.remoteAddress << ":" << conn.remotePort
               << " (" << conn.protocol << " - " << conn.state << ")\n";
    }
    
    report << "\nOpen Handles:\n";
    report << "------------\n";
    for (const auto& handle : info.openHandles) {
        report << handle << "\n";
    }
}