#pragma once
#include <string>
#include <vector>
#include <pcap.h>
#include <boost/asio.hpp>
#include <stdexcept>

class NetworkAnalyzer {
public:
    struct NetworkData {
        std::string source_ip;
        std::string dest_ip;
        int source_port;
        int dest_port;
        std::vector<uint8_t> payload;
    };

    struct NetworkError {
        std::string message;
        int error_code;
        std::string timestamp;
    };

    NetworkAnalyzer();
    ~NetworkAnalyzer();
    
    void initializeCapture(const std::string& interface_name);
    std::vector<NetworkData> captureTraffic(int duration_seconds);
    std::vector<std::string> listInterfaces();
    bool analyzePacket(const std::string& target_ip);
    void stopCapture();
    std::vector<NetworkError> getLastErrors();
    void clearErrors();

    // Nouvelles méthodes de gestion d'erreurs
    bool isInitialized() const { return initialized; }
    std::string getLastErrorMessage() const { return last_error_message; }
    void setLastError(const std::string& message, int code);

private:
    pcap_t* handle;
    std::string interface;
    std::vector<std::string> captured_packets;
    bool is_capturing;
    bool initialized;
    std::string last_error_message;
    std::vector<NetworkError> error_history;
    
    // Méthodes privées de gestion d'erreurs
    void handlePcapError(const char* error_msg);
    void logError(const NetworkError& error);
};