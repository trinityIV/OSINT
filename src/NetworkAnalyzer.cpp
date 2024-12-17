#include "../include/NetworkAnalyzer.h"
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <map>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <regex>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

class PacketAnalyzer {
public:
    struct PacketInfo {
        std::string protocol;
        std::string source;
        std::string destination;
        size_t size;
        std::string timestamp;
        std::vector<uint8_t> payload;
    };

    static PacketInfo analyzePacket(const uint8_t* packet, size_t length) {
        PacketInfo info;
        // Analyse des en-têtes IP et TCP/UDP
        if (length >= sizeof(struct ip)) {
            const struct ip* ip_header = reinterpret_cast<const struct ip*>(packet);
            char src[INET_ADDRSTRLEN];
            char dst[INET_ADDRSTRLEN];
            
            inet_ntop(AF_INET, &(ip_header->ip_src), src, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(ip_header->ip_dst), dst, INET_ADDRSTRLEN);
            
            info.source = src;
            info.destination = dst;
            info.size = length;
            
            switch (ip_header->ip_p) {
                case IPPROTO_TCP:
                    info.protocol = "TCP";
                    break;
                case IPPROTO_UDP:
                    info.protocol = "UDP";
                    break;
                case IPPROTO_ICMP:
                    info.protocol = "ICMP";
                    break;
                default:
                    info.protocol = "Unknown";
            }
        }
        return info;
    }
};

NetworkAnalyzer::NetworkAnalyzer() : handle(nullptr), is_capturing(false) {
    initializeWinSock();
}

NetworkAnalyzer::~NetworkAnalyzer() {
    if (handle) {
        pcap_close(handle);
    }
    WSACleanup();
}

void NetworkAnalyzer::initializeWinSock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("Failed to initialize WinSock");
    }
}

std::vector<NetworkAnalyzer::NetworkData> NetworkAnalyzer::captureTraffic(int duration_seconds) {
    std::vector<NetworkData> traffic_data;
    struct pcap_pkthdr header;
    const u_char* packet;
    
    std::map<std::string, int> protocol_stats;
    std::map<std::string, size_t> ip_stats;
    
    auto start_time = std::chrono::steady_clock::now();
    
    while (is_capturing) {
        auto current_time = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count() >= duration_seconds) {
            break;
        }
        
        packet = pcap_next(handle, &header);
        if (packet != nullptr) {
            auto packet_info = PacketAnalyzer::analyzePacket(packet, header.len);
            
            NetworkData data;
            data.source_ip = packet_info.source;
            data.dest_ip = packet_info.destination;
            data.payload.assign(packet, packet + header.len);
            
            // Mise à jour des statistiques
            protocol_stats[packet_info.protocol]++;
            ip_stats[data.source_ip] += header.len;
            
            traffic_data.push_back(data);
            
            // Log en temps réel
            logPacket(packet_info);
        }
    }
    
    // Génération du rapport
    generateTrafficReport(protocol_stats, ip_stats);
    
    return traffic_data;
}

void NetworkAnalyzer::logPacket(const PacketAnalyzer::PacketInfo& packet) {
    static std::ofstream log_file("data/packet_log.txt", std::ios::app);
    log_file << std::put_time(std::localtime(&std::time(nullptr)), "%Y-%m-%d %H:%M:%S")
             << " | " << std::setw(5) << packet.protocol
             << " | " << std::setw(15) << packet.source
             << " -> " << std::setw(15) << packet.destination
             << " | Size: " << packet.size << " bytes\n";
}

void NetworkAnalyzer::generateTrafficReport(
    const std::map<std::string, int>& protocol_stats,
    const std::map<std::string, size_t>& ip_stats) {
    
    std::ofstream report("data/traffic_report.txt");
    report << "=== Traffic Analysis Report ===\n\n";
    
    // Statistiques des protocoles
    report << "Protocol Statistics:\n";
    for (const auto& [protocol, count] : protocol_stats) {
        report << std::setw(10) << protocol << ": " << count << " packets\n";
    }
    
    // Top 10 des IPs les plus actives
    report << "\nTop 10 Active IPs:\n";
    std::vector<std::pair<std::string, size_t>> ip_list(ip_stats.begin(), ip_stats.end());
    std::sort(ip_list.begin(), ip_list.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    int count = 0;
    for (const auto& [ip, bytes] : ip_list) {
        if (count++ >= 10) break;
        report << std::setw(15) << ip << ": " << bytes / 1024.0 << " KB\n";
    }
}

bool NetworkAnalyzer::analyzePacket(const std::string& target_ip) {
    if (!is_capturing || !handle) {
        return false;
    }
    
    struct bpf_program fp;
    char filter_exp[64];
    snprintf(filter_exp, sizeof(filter_exp), "host %s", target_ip.c_str());
    
    if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        return false;
    }
    
    if (pcap_setfilter(handle, &fp) == -1) {
        pcap_freecode(&fp);
        return false;
    }
    
    pcap_freecode(&fp);
    return true;
}

std::vector<std::string> NetworkAnalyzer::scanPorts(const std::string& target_ip, 
                                                   const std::vector<int>& ports) {
    std::vector<std::string> open_ports;
    
    for (int port : ports) {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            continue;
        }
        
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        inet_pton(AF_INET, target_ip.c_str(), &server.sin_addr);
        
        // Set socket timeout
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
        
        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == 0) {
            std::string service = getServiceName(port);
            open_ports.push_back(std::to_string(port) + " (" + service + ")");
        }
        
        closesocket(sock);
    }
    
    return open_ports;
}

std::string NetworkAnalyzer::getServiceName(int port) {
    switch (port) {
        case 21: return "FTP";
        case 22: return "SSH";
        case 23: return "Telnet";
        case 25: return "SMTP";
        case 53: return "DNS";
        case 80: return "HTTP";
        case 443: return "HTTPS";
        case 445: return "SMB";
        case 3389: return "RDP";
        case 8080: return "HTTP-Proxy";
        default: return "Unknown";
    }
}