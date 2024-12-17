#pragma once
#include <Windows.h>
#include "WebServer.h"
#include "ConsoleUtils.h"
#include "AsciiArt.h"
#include <lua.h>



class OSINTTool {
public:
    // Types et structures
    struct NetworkInfo {
        std::string ip;
        std::string mac;
        std::string hostname;
        std::vector<int> openPorts;
		std::string subnetMask;
		std::string macAddress;
		std::string defaultGateway;
		std::vector<std::string> dnsServers;
		std::string error;
		std::string resolveDNS(const std::string& hostname);
		std::vector<int> scanPorts(const std::string& ip, int startPort, int endPort);
		bool testConnection(const std::string& host, int port);
		std::string getMACAddress(const std::string& ip);
		std::string getSubnetMask(const std::string& ip);
		std::string getDefaultGateway(const std::string& ip);
		std::vector<std::string> getDNSServers(const std::string& ip);

    };

    struct ProcessInfo {
        DWORD pid;
        DWORD parentPid;
		std::string name;
		std::string path;
		std::string commandLine;
		std::string owner;
		int threads;
		int priority;
		std::string startTime;
		std::string endTime;
		std::string duration;
		std::vector<std::string> openFiles;
		std::map<std::string, std::string> modules;
		std::string error;
		std::map<std::string, std::string> getModules(DWORD pid);
		std::vector<std::string> getOpenFiles(DWORD pid);
		std::string getOwner(DWORD pid);
		std::string getStartTime(DWORD pid);
		std::string getEndTime(DWORD pid);
		std::string getDuration(DWORD pid);
		int getPriority(DWORD pid);
		int getThreadCount(DWORD pid);
		std::string getCommandLine(DWORD pid);
		std::string getPath(DWORD pid);
		std::string getName(DWORD pid);
		DWORD getParentPid(DWORD pid);
		std::string getError();
        //analyzeProcess
		ProcessInfo analyzeProcess(const std::string& name);
		std::vector<ProcessInfo> listProcesses();
		bool killProcess(DWORD pid);
		std::map<std::string, std::string> getProcessModules(DWORD pid);




    };

    struct SystemInfo {
        std::string osVersion;
        std::string architecture;
        size_t totalMemory;
        size_t availableMemory;
        int processorCount;
        std::string computerName;
		std::string userName;
		std::string domainName;
		std::string systemDirectory;
		std::string bootTime;
		std::string publicIP;
		std::string privateIP;
		std::string macAddress;
		std::string subnetMask;
		std::string defaultGateway;
		std::vector<std::string> dnsServers;
		std::string currentDirectory;
		std::string appDataPath;
		std::string tempPath;
		std::string programFilesPath;
		std::string systemPath;
		std::string desktopPath;
		std::string documentsPath;
		std::string picturesPath;
		std::string musicPath;
		std::string videosPath;
		std::string downloadsPath;
		std::string favoritesPath;
		std::string contactsPath;
		std::string publicPath;
		std::string cachePath;
		std::string cookiesPath;
		std::string historyPath;
		std::string internetCachePath;
		std::string recentPath;
		std::string sendToPath;
		std::string startMenuPath;
		std::string startupPath;
		std::string localAppDataPath;
		std::string printerSpoolPath;
		std::string windowsPath;
		std::string windowsSystemPath;
		std::string windowsTempPath;
		std::string programDataPath;
		std::string commonAppDataPath;
		std::string commonProgramFilesPath;
		std::string commonStartMenuPath;
		std::string commonStartupPath;
        //osName
		std::string osName;
		std::string cpuName;
		std::string gpuName;
		std::string memoryTotal;
		std::string memoryAvailable;
		std::string memoryUsed;
		std::string memoryUsage;
		std::string memoryFree;
		std::string memoryUsagePercentage;
		std::string memoryFreePercentage;
		std::string memoryUsedPercentage;
		std::string memoryAvailablePercentage;
		std::string memoryTotalPercentage;
		std::string memoryTotalGB;
		std::string memoryAvailableGB;
		std::string memoryUsedGB;
		std::string memoryFreeGB;
		std::string memoryUsageGB;
		std::string memoryUsagePercentageGB;
		std::string memoryFreePercentageGB;
		std::string memoryUsedPercentageGB;

		//hostname
		std::string hostname;
		std::string username;
		std::string ipAddress;
		std::vector<std::pair<std::string, std::string>> drives;
		std::vector<std::string> errors;
		std::string error;
    };

    struct ScriptResult {
        bool success;
        std::string output;
        std::string error;
		lua_State* luaState;



    };

    // Constructeur et destructeur
    OSINTTool();
    ~OSINTTool();

    // Menu principal
    void showMenu();
    void processChoice(int choice);

    // Fonctions d'analyse principales
    void networkAnalysis();
    void domainAnalysis();
    void profileAnalysis();
	void applicationAudit();
	void generateReport();


    // Interface Web
    bool startWebInterface(int port = 0);
    void stopWebInterface();
    std::string getWebUrl() const;

    // Fonctions de script
    ScriptResult executePythonScript(const std::string& script);
    ScriptResult executePythonFile(const std::string& filename);
    ScriptResult executeLuaScript(const std::string& script);
    ScriptResult executeLuaFile(const std::string& filename);

private:
    // Fonctions utilitaires
    void initializeDatabase();
    void loadConfiguration();
    std::string getCurrentTimestamp();
    bool saveToFile(const std::string& filename, const std::string& content);
    std::string loadFromFile(const std::string& filename);

    // Fonctions d'analyse réseau
    NetworkInfo scanNetwork(const std::string& targetIp);
    std::vector<int> scanPorts(const std::string& ip, int startPort, int endPort);
    bool testConnection(const std::string& host, int port);
    std::string resolveDNS(const std::string& hostname);

    // Fonctions d'analyse de processus
    ProcessInfo getProcessInfo(DWORD pid);
    std::vector<ProcessInfo> listProcesses();
    bool killProcess(DWORD pid);
    std::map<std::string, std::string> getProcessModules(DWORD pid);

    // Fonctions système
    SystemInfo getSystemInformation();
    std::map<std::string, std::string> getEnvironmentVariables();
    std::vector<std::string> getInstalledSoftware();
    bool isAdministrator();

    // Gestionnaires d'API Web
    void handleNetworkTest(const httplib::Request& req, httplib::Response& res);
    void handleProcessAnalysis(const httplib::Request& req, httplib::Response& res);
    void handleSystemInfo(const httplib::Request& req, httplib::Response& res);
    void handleScriptExecution(const httplib::Request& req, httplib::Response& res);
    void handleError(httplib::Response& res, const std::string& message, int code);

    // Fonctions de script privées
    void initializePython();
    void finalizePython();
    void initializeLua();
    void finalizeLua();
    
    // Membres privés
    std::unique_ptr<WebServer> webServer;
    Json::Value config;
    bool webInterfaceActive;
    std::string databasePath;
    std::string configPath;
    std::string reportPath;
    std::string scriptPath;

    // Environnements de script
    lua_State* luaState;
    bool pythonInitialized;

    // Cache et état
    SystemInfo cachedSystemInfo;
    std::chrono::system_clock::time_point lastSystemInfoUpdate;
    static constexpr auto CACHE_DURATION = std::chrono::minutes(5);
	static constexpr auto SCRIPT_TIMEOUT = std::chrono::seconds(10);
	static constexpr auto SCRIPT_INTERVAL = std::chrono::seconds(5);
	static constexpr auto SCRIPT_MAX_DURATION = std::chrono::seconds(30);
	static constexpr auto SCRIPT_MAX_OUTPUT = 1024;
	static constexpr auto SCRIPT_MAX_ERROR = 1024;
	static constexpr auto SCRIPT_MAX_LINES = 100;
	static constexpr auto SCRIPT_MAX_LINE_LENGTH = 1024;
	static constexpr auto SCRIPT_MAX_OUTPUT_LENGTH = 1024;
	static constexpr auto SCRIPT_MAX_ERROR_LENGTH = 1024;
	static constexpr auto SCRIPT_MAX_OUTPUT_LINES = 100;
	static constexpr auto SCRIPT_MAX_ERROR_LINES = 100;
	static constexpr auto SCRIPT_MAX_OUTPUT_LINE_LENGTH = 1024;
	static constexpr auto SCRIPT_MAX_ERROR_LINE_LENGTH = 1024;
	static constexpr auto SCRIPT_MAX_OUTPUT_LENGTH = 1024;
	static constexpr auto SCRIPT_MAX_ERROR_LENGTH = 1024;
	static constexpr auto SCRIPT_MAX_OUTPUT_LINES = 100;
	static constexpr auto SCRIPT_MAX_ERROR_LINES = 100;
	static constexpr auto SCRIPT_MAX_OUTPUT_LINE_LENGTH = 1024;


	//lastActivity
	std::chrono::system_clock::time_point lastActivity;

	//lastErrorMessage
	std::string lastErrorMessage;

	//errorQueue
	std::queue<WebServer::ServerError> errorQueue;

};