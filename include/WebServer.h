#pragma once
#include <string>
#include <mutex>
#include <httplib.h>
#include <boost/asio.hpp>
#include <windows.h>
#include <json/json.h>
#include <iphlpapi.h>
#include <queue>
#include <sqlite3.h>
#include "ConsoleUtils.h"

class WebServer {
public:
    struct ServerError {
        std::string message;
        std::string timestamp;
        int code;
        //lastActivity
		std::chrono::system_clock::time_point lastActivity;
    };

    WebServer(int port = 0);  // 0 pour port automatique
    ~WebServer();

    bool start();
    void stop();
    int getPort() const { return port; }
    std::string getPublicUrl() const;
    void setStaticDir(const std::string& path);
    void addRoute(const std::string& path,std::function<void(const httplib::Request&, httplib::Response&)> handler);
	void set_before_handler(const httplib::Server::Handler& handler);
    
    // Nouvelles méthodes de gestion d'erreurs
    std::vector<ServerError> getLastErrors(int count = 10) const;
    void clearErrors();
    bool isRunning() const { return running; }
    std::string getLastErrorMessage() const;

private:
    bool configureFirewall();
    bool openPort();
    bool setupUPnP();
    std::string getPublicIP();
    int findAvailablePort();
    void startServer();
    void logError(const std::string& message, int code);
    void monitorConnections();
    bool checkTimeout();

    std::unique_ptr<httplib::Server> server;
    std::unique_ptr<std::thread> serverThread;
    std::unique_ptr<std::thread> monitorThread;


    
    mutable std::mutex serverMutex;
    mutable std::mutex errorMutex;
    std::condition_variable stopCondition;
    std::atomic<bool> running{false};
    std::atomic<bool> shouldStop{false};
    
    int port;
    std::string publicIP;
    std::string lastErrorMessage;
    
    // File d'erreurs avec timestamp
    std::queue<ServerError> errorQueue;
    const size_t MAX_ERROR_QUEUE_SIZE = 100;
    
    // Timeouts et limites
    const std::chrono::seconds CONNECTION_TIMEOUT{30};
    const std::chrono::seconds MONITOR_INTERVAL{5};
    const int MAX_CONNECTIONS = 100;
    
    // État du serveur
    std::atomic<int> activeConnections{0};
    std::chrono::system_clock::time_point lastActivity;

	//set_before_handler handler for server 
	httplib::Server::Handler before_handler;

	// Configuration du serveur
	httplib::Server::Handler getBeforeHandler() const { return before_handler; }
	void setBeforeHandler(const httplib::Server::Handler& handler) { before_handler = handler; }



};