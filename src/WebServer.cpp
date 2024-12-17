#include <webserver.h>
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/upnperrors.h>
#include <Python.h>


#define MAX_CONNECTIONS 100
#define CONNECTION_TIMEOUT std::chrono::minutes(5)
#define MONITOR_INTERVAL std::chrono::seconds(5)
#define MAX_ERROR_QUEUE_SIZE 100





WebServer::WebServer(int port) : port(port) {
    server = std::make_unique<httplib::Server>();
    lastActivity = std::chrono::system_clock::now();
    
    // Configuration du serveur
    server->set_keep_alive_max_count(MAX_CONNECTIONS);
    server->set_read_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
    server->set_write_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
    


    server->set_post_routing_handler([this](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(serverMutex);
        activeConnections--;
        return true;
    });

	server->set_error_handler([this](const httplib::Request& req, httplib::Response& res) {
		std::lock_guard<std::mutex> lock(serverMutex);
		activeConnections--;
		return true;
		});



    //lastActivity
	lastActivity = std::chrono::system_clock::now();
	activeConnections++;
	//lastErrorMessage
	lastErrorMessage.clear();
	//errorQueue
	std::queue<ServerError>().swap(errorQueue);
	// Timeouts et limites
	// Configuration du serveur
	server->set_keep_alive_max_count(MAX_CONNECTIONS);
	server->set_read_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	server->set_write_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	// Configuration du serveur
	server->set_keep_alive_max_count(MAX_CONNECTIONS);
	server->set_read_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	server->set_write_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	// Configuration du serveur
	server->set_keep_alive_max_count(MAX_CONNECTIONS);
	server->set_read_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	server->set_write_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	// Configuration du serveur
	server->set_keep_alive_max_count(MAX_CONNECTIONS);
	server->set_read_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	server->set_write_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	// Configuration du serveur
	server->set_keep_alive_max_count(MAX_CONNECTIONS);
	server->set_read_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	server->set_write_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	// Configuration du serveur
	server->set_keep_alive_max_count(MAX_CONNECTIONS);
	server->set_read_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	server->set_write_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	// Configuration du serveur
	server->set_keep_alive_max_count(MAX_CONNECTIONS);
	server->set_read_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	server->set_write_timeout(static_cast<int>(CONNECTION_TIMEOUT.count()));
	// Configuration du serveur
	server->set_keep_alive_max_count(MAX_CONNECTIONS);

}

WebServer::~WebServer() {
    stop();
}

bool WebServer::start() {
    try {
        std::lock_guard<std::mutex> lock(serverMutex);
        if (running) return true;

        // Trouve un port disponible si nécessaire
        if (port == 0) {
            port = findAvailablePort();
            if (port == 0) {
                logError("Failed to find available port", 1001);
                return false;
            }
        }

        // Configure le pare-feu Windows
        if (!configureFirewall()) {
            logError("Failed to configure firewall", 1002);
            return false;
        }

        // Ouvre le port sur le routeur via UPnP
        if (!setupUPnP()) {
            logError("Failed to setup UPnP", 1003);
            return false;
        }

        // Obtient l'IP publique
        publicIP = getPublicIP();
        if (publicIP.empty()) {
            logError("Failed to get public IP", 1004);
            return false;
        }

        // Démarre le serveur dans un thread séparé
        serverThread = std::make_unique<std::thread>(&WebServer::startServer, this);
        monitorThread = std::make_unique<std::thread>(&WebServer::monitorConnections, this);
        
        running = true;
        return true;
    }
    catch (const std::exception& e) {
        logError(std::string("Server start failed: ") + e.what(), 1000);
        return false;
    }
}

void WebServer::stop() {
    try {
        {
            std::lock_guard<std::mutex> lock(serverMutex);
            if (!running) return;
            shouldStop = true;
        }
        
        stopCondition.notify_all();
        
        if (server) {
            server->stop();
        }
        
        if (serverThread && serverThread->joinable()) {
            serverThread->join();
        }
        
        if (monitorThread && monitorThread->joinable()) {
            monitorThread->join();
        }
        
        running = false;
        shouldStop = false;
    }
    catch (const std::exception& e) {
        logError(std::string("Server stop failed: ") + e.what(), 2000);
    }
}

std::string WebServer::getPublicUrl() const
{
	return "http://" + publicIP + ":" + std::to_string(port);

}

void WebServer::setStaticDir(const std::string& path)
{
	server->set_base_dir(path.c_str());

}

void WebServer::addRoute(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler)
{
	server->Post(path.c_str(), handler);
	server->Get(path.c_str(), handler);

}

void WebServer::set_before_handler(const httplib::Server::Handler& handler)
{
	// Définit un gestionnaire pour les requêtes entrantes avant le routage des routes spécifiques 
	server->set_file_request_handler(handler);

	// Mise à jour de l'heure de la dernière activité
	lastActivity = std::chrono::system_clock::now();
	activeConnections++;

}


std::string WebServer::getPublicIP()
{
	// Obtient l'adresse IP publique en utilisant un service Web
	httplib::Client client("api.ipify.org");
	auto res = client.Get("/");
	if (res && res->status == 200) {
		return res->body;
	}
	else {
		return "";
	}

}

int WebServer::findAvailablePort()
{
	// Création d'un générateur de nombres aléatoires
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(1024, 65535);
	// Essaie des ports aléatoires jusqu'à en trouver un disponible
	for (int i = 0; i < 10; ++i) {
		int p = dis(gen);
		if (server->bind_to_any_port("", p)) {
			return p;
		}
	}
	return 0;

}

void WebServer::startServer()
{
	// Démarrage du serveur sur le port spécifié
	server->listen("", port);
}

void WebServer::logError(const std::string& message, int code) {
    std::lock_guard<std::mutex> lock(errorMutex);
    
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S");
    
    ServerError error{message, ss.str(), code};
    errorQueue.push(error);
    
    if (errorQueue.size() > MAX_ERROR_QUEUE_SIZE) {
        errorQueue.pop();
    }
    
    lastErrorMessage = message;
}

std::vector<WebServer::ServerError> WebServer::getLastErrors(int count) const {
    std::lock_guard<std::mutex> lock(errorMutex);
    std::vector<ServerError> errors;
    auto tempQueue = errorQueue;
    
    while (!tempQueue.empty() && count > 0) {
        errors.push_back(tempQueue.front());
        tempQueue.pop();
        count--;
    }
    
    return errors;
}

void WebServer::clearErrors() {
    std::lock_guard<std::mutex> lock(errorMutex);
    std::queue<ServerError>().swap(errorQueue);
    lastErrorMessage.clear();
}

std::string WebServer::getLastErrorMessage() const
{
	char buffer[100];
	std::lock_guard<std::mutex> lock(errorMutex);
	if (lastErrorMessage.empty()) {
		return "No errors";
	}
	else {
		return lastErrorMessage;
	}
}

bool WebServer::configureFirewall()
{
	// Configuration du pare-feu Windows pour autoriser le trafic sur le port spécifié
	std::string command = "netsh advfirewall firewall add rule name=\"WebServer\" dir=in action=allow protocol=TCP localport=" + std::to_string(port);
	return system(command.c_str()) == 0;
}

char lanaddr[64];
//ERRORSIZE
#define ERROR_SIZE 64


bool WebServer::openPort()
{

	strcat(lanaddr, "");
	struct UPNPDev* devlist = 0;
	struct UPNPUrls urls;
	struct IGDdatas data;
	int r;
	const char* multicastif = 0;
	const char* minissdpdpath = 0;
	char lanaddr[64];
	char externalIPAddress[40];
	unsigned short port = 0;
	const char* desc = 0;
	int upnpErrorCode = 0;
	const char* upnpErrorMsg = 0;
	int i = 0;
	int nerror = 0;
    int error;
	int rport = 0;
	const char* leaseDuration = "0";
	const char* portStr = "8080";
	const char* descStr = "Port mapping";
	const char* proto = "TCP";
	const char* intClient = "";
	const char* intPort = "0";
	const char* extPort = "0";
	const char* enabled = "1";
	const char* rhost = "";
	char wanaddr[64];


    devlist = upnpDiscover(2000, multicastif, minissdpdpath, 0, 0, error, 0);

	if (devlist) {
		r = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, 0,wanaddr,0);

		if (r == 1) {
			r = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype, extPort, intPort, lanaddr, descStr, proto, 0, leaseDuration);
			if (r != UPNPCOMMAND_SUCCESS) {
				nerror = 1;
			}
			else {
				rport = atoi(extPort);
			}
			freeUPNPDevlist(devlist);
            urls.controlURL;
			urls.rootdescURL;
			urls.ipcondescURL;

		
		}
		else {
			freeUPNPDevlist(devlist);
			nerror = 1;
		}
	}
    else {
        nerror = 1;
    }
}

bool WebServer::setupUPnP()
{
	// Ouvre le port sur le routeur via UPnP
	return openPort();

}

void WebServer::monitorConnections() {
    while (!shouldStop) {
        std::this_thread::sleep_for(MONITOR_INTERVAL);
        
        if (checkTimeout()) {
            logError("Server timeout detected", 3000);
            stop();
            break;
        }
    }
}

bool WebServer::checkTimeout() {
    std::lock_guard<std::mutex> lock(serverMutex);
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastActivity);
    
    return duration > CONNECTION_TIMEOUT && activeConnections > 0;
}