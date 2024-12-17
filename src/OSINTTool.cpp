#include "OSINTTool.h"
#include <conio.h>
#include <filesystem>
#include <fstream>
#include <lauxlib.h>
#include <lualib.h>
#include <sqlite3.h>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <Python.h>




namespace {
	const std::string CONFIG_FILE = "config.json";
}

//appDataPath 
enum appDataPath {
	CSIDL_APPDATA = 0x001a
};

//db 
sqlite3* db = nullptr;

//SHGetFolderPathA
HRESULT SHGetFolderPathA(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath);




using namespace std;

namespace fs = std::filesystem;

std::string OSINTTool::getCurrentTimestamp() {
	auto now = std::chrono::system_clock::now();
	auto nowTime = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S");
	return ss.str();
}

bool OSINTTool::saveToFile(const std::string& filename, const std::string& content)
{
	std::ofstream file(filename);
	if (file.is_open()) {
		file << content;
		file.close();
		return true;
	}
	return false;
}

std::string OSINTTool::loadFromFile(const std::string& filename)
{
	std::ifstream(file);
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close();
		return buffer.str();
	}
	return "";

}

OSINTTool::NetworkInfo OSINTTool::scanNetwork(const std::string& targetIp)
{
	return NetworkInfo();
}

std::vector<int> OSINTTool::scanPorts(const std::string& ip, int startPort, int endPort)
{
	return std::vector<int>();
}

bool OSINTTool::testConnection(const std::string& host, int port)
{
	return false;
}

std::string OSINTTool::resolveDNS(const std::string& hostname)
{
	return std::string();
}

OSINTTool::ProcessInfo OSINTTool::getProcessInfo(DWORD pid)
{
	return ProcessInfo();
}

std::vector<OSINTTool::ProcessInfo> OSINTTool::listProcesses()
{
	return std::vector<ProcessInfo>();
}

bool OSINTTool::killProcess(DWORD pid)
{
	return false;
}

std::map<std::string, std::string> OSINTTool::getProcessModules(DWORD pid)
{
	return std::map<std::string, std::string>();
}

OSINTTool::SystemInfo OSINTTool::getSystemInformation()
{
	return SystemInfo();
}

std::map<std::string, std::string> OSINTTool::getEnvironmentVariables()
{
	return std::map<std::string, std::string>();
}

std::vector<std::string> OSINTTool::getInstalledSoftware()
{
	return std::vector<std::string>();
}

bool OSINTTool::isAdministrator()
{
	return false;
}

OSINTTool::NetworkInfo scanNetwork(const std::string& targetIp)
{
	//networkInfo
	OSINTTool::NetworkInfo* networkInfo = new OSINTTool::NetworkInfo();
	networkInfo->ip = targetIp;
	networkInfo->hostname = networkInfo->resolveDNS(targetIp);
	networkInfo->macAddress = networkInfo->getMACAddress(targetIp);
	networkInfo->subnetMask = networkInfo->getSubnetMask(targetIp);
	networkInfo->defaultGateway = networkInfo->getDefaultGateway(targetIp);
	networkInfo->dnsServers = networkInfo->getDNSServers(targetIp);
	networkInfo->openPorts = networkInfo->scanPorts(targetIp, 1, 1024);
	return *networkInfo;


}



void OSINTTool::initializeDatabase() {
	// Initialiser la base de données SQLite
	int result = sqlite3_open("osint.db", &db);
	if (result != SQLITE_OK) {
		std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << "\n";
		sqlite3_close(db);
		db = nullptr;
	}
	else {
		std::cout << "Database opened successfully\n";
	}

}

void OSINTTool::loadConfiguration()
{
	// Charger la configuration à partir du fichier JSON
	std::ifstream file(CONFIG_FILE);
	if (file.is_open()) {
		file >> config;
		file.close();
		std::cout << "Configuration loaded successfully\n";
	}
	else {
		std::cerr << "Failed to load configuration\n";
	}

}



OSINTTool::OSINTTool() : webInterfaceActive(false), luaState(nullptr), pythonInitialized(false) {
    // Initialisation des chemins
    char appData[MAX_PATH];
	string appDataPath = getenv("APPDATA");
	string logPath;
	scriptPath = appDataPath + "\\OSINTTool\\scripts";
	reportPath = appDataPath + "\\OSINTTool\\reports";
	logPath = appDataPath + "\\OSINTTool\\logs";
	SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appData);

	// Créer les dossiers nécessaires
	fs::create_directories(scriptPath);
	fs::create_directories(reportPath);
	fs::create_directories(logPath);

	// Initialiser la base de données
    initializeDatabase();
    loadConfiguration();
    initializePython();
    initializeLua();

	// Initialiser le serveur Web
	webServer = std::make_unique<WebServer>();
	webServer->setStaticDir("web/static");
	webServer->addRoute("/api/network/test", [this](const httplib::Request& req, httplib::Response& res) {
		handleNetworkTest(req, res);
		});
	webServer->addRoute("/api/process/analyze", [this](const httplib::Request& req, httplib::Response& res) {
		handleProcessAnalysis(req, res);
		});
	webServer->addRoute("/api/system/info", [this](const httplib::Request& req, httplib::Response& res) {
		handleSystemInfo(req, res);
		});
	webServer->addRoute("/api/script/execute", [this](const httplib::Request& req, httplib::Response& res) {
		handleScriptExecution(req, res);
		});
	webServer->set_before_handler([this](const httplib::Request& req, httplib::Response& res) {
		lastActivity = std::chrono::system_clock::now();
		});
	webServer->addRoute("/api/errors", [this](const httplib::Request& req, httplib::Response& res) {
		handleError(res, "Not implemented", 501);
		});
	webServer->start();
	webInterfaceActive = true;



}

OSINTTool::~OSINTTool() {
    finalizePython();
    finalizeLua();
	if (db) {
		sqlite3_close(db);
		db = nullptr;
	}


}

void OSINTTool::showMenu()
{
	std::cout << "\n=== BadHabit OSINT Tool ===\n";
	std::cout << "1. Network Analysis\n";
	std::cout << "2. Domain Analysis\n";
	std::cout << "3. Profile Analysis\n";
	std::cout << "4. Application Audit\n";
	std::cout << "5. Generate Report\n";
	std::cout << "6. Start Web Interface\n";
	std::cout << "0. Exit\n";
	std::cout << "\nChoice: ";

}

void OSINTTool::processChoice(int choice)
{
	switch (choice) {
	case 1:
		networkAnalysis();
		break;
	case 2:
		domainAnalysis();
		break;
	case 3:
		profileAnalysis();
		break;
	case 4:
		applicationAudit();
		break;
	case 5:
		generateReport();
		break;
	case 6: {
		std::cout << "Starting web interface...\n";
		if (startWebInterface()) {
			std::cout << "Web interface started at: " << getWebUrl() << "\n";
			std::cout << "Press any key to stop the web interface...\n";
			_getch();  // Attend une touche
			stopWebInterface();
		}
		else {
			std::cout << "Failed to start web interface.\n";
		}
		break;
	}
	case 0:
		break;
	default:
		std::cout << "Invalid choice\n";
		break;
	}

}

void OSINTTool::networkAnalysis()
{
	std::string targetIp;
	std::cout << "Enter target IP address: ";
	std::cin >> targetIp;
	NetworkInfo networkInfo = scanNetwork(targetIp);
	std::cout << "IP: " << networkInfo.ip << "\n";
	std::cout << "Hostname: " << networkInfo.hostname << "\n";
	std::cout << "MAC Address: " << networkInfo.macAddress << "\n";
	std::cout << "Subnet Mask: " << networkInfo.subnetMask << "\n";
	std::cout << "Default Gateway: " << networkInfo.defaultGateway << "\n";
	std::cout << "DNS Servers:\n";
	for (const std::string& dns : networkInfo.dnsServers) {
		std::cout << "  " << dns << "\n";
	}
	std::cout << "Open Ports:\n";
	for (int port : networkInfo.openPorts) {
		std::cout << "  " << port << "\n";
	}

}

void OSINTTool::domainAnalysis()
{
	std::string domain;
	std::cout << "Enter domain name: ";
	std::cin >> domain;
	std::string ip = resolveDNS(domain);
	std::cout << "IP Address: " << ip << "\n";

}

void OSINTTool::profileAnalysis()
{
	// Analyser le profil utilisateur actuel du système
	SystemInfo systemInfo = getSystemInformation();
	std::cout << "System Information:\n";
	std::cout << "  OS: " << systemInfo.osName << " " << systemInfo.osVersion << "\n";
	std::cout << "  Hostname: " << systemInfo.hostname << "\n";
	std::cout << "  Username: " << systemInfo.username << "\n";
	std::cout << "  IP Address: " << systemInfo.ipAddress << "\n";
	std::cout << "  MAC Address: " << systemInfo.macAddress << "\n";
	std::cout << "  CPU: " << systemInfo.cpuName << "\n";
	std::cout << "  Memory: " << systemInfo.memoryTotal << " MB\n";
	std::cout << "  GPU: " << systemInfo.gpuName << "\n";
	std::cout << "  Drives:\n";
	for (const auto& drive : systemInfo.drives) {
		std::cout << "    " << drive.first << " - " << drive.second << " GB\n";
	}
	std::cout << "  Errors:\n";
	for (const std::string& error : systemInfo.errors) {
		std::cout << "    " << error << "\n";
	}

}

void OSINTTool::applicationAudit()
{
	// Analyser les applications installées sur le système
	std::vector<std::string> software = getInstalledSoftware();
	std::cout << "Installed Software:\n";
	for (const std::string& app : software) {
		std::cout << "  " << app << "\n";
	}

}

void OSINTTool::generateReport()
{
	// Générer un rapport sur les informations collectées
	std::string filename = reportPath + "\\report_" + getCurrentTimestamp() + ".txt";
	std::ofstream file(filename);
}

void OSINTTool::initializePython() {
    if (!pythonInitialized) {
        Py_Initialize();
        pythonInitialized = true;
        
        // Ajouter le dossier des scripts au path Python
        PyRun_SimpleString("import sys");
        std::string pythonPath = "sys.path.append('" + scriptPath + "')";
        PyRun_SimpleString(pythonPath.c_str());
    }
}

void OSINTTool::finalizePython() {
    if (pythonInitialized) {
        Py_Finalize();
        pythonInitialized = false;
    }
}

void OSINTTool::initializeLua()
{
	luaState = luaL_newstate();
	luaL_openlibs(luaState);

}

void OSINTTool::finalizeLua() {
    if (luaState) {
        lua_close(luaState);
        luaState = nullptr;
    }
}

void OSINTTool::handleNetworkTest(const httplib::Request& req, httplib::Response& res)
{
	Json::Value result;
	result["timestamp"] = getCurrentTimestamp();
	std::string targetIp = req.get_param_value("ip");
	NetworkInfo networkInfo = scanNetwork(targetIp);
	result["network_info"]["ip"] = networkInfo.ip;
	result["network_info"]["hostname"] = networkInfo.hostname;
	result["network_info"]["mac_address"] = networkInfo.macAddress;
	result["network_info"]["subnet_mask"] = networkInfo.subnetMask;
	result["network_info"]["default_gateway"] = networkInfo.defaultGateway;
	result["network_info"]["dns_servers"] = Json::Value(Json::arrayValue);
	result["network_info"]["open_ports"] = Json::Value(Json::arrayValue);
	for (int port : networkInfo.openPorts) {
		result["network_info"]["open_ports"].append(port);
	}
	res.set_content(result.toStyledString(), "application/json");


}

//analyzeProcess

void OSINTTool::handleProcessAnalysis(const httplib::Request& req, httplib::Response& res)
{
	Json::Value result;
	result["timestamp"] = getCurrentTimestamp();
	std::string processName = req.get_param_value("name");
	ProcessInfo processInfo;
	processInfo.analyzeProcess(processName);
	result["process_info"]["pid"], processInfo.pid;
	result["process_info"]["name"] = processInfo.name;
	result["process_info"]["path"] = processInfo.path;
	result["process_info"]["command_line"] = processInfo.commandLine;
	result["process_info"]["owner"] = processInfo.owner;
	result["process_info"]["threads"] = processInfo.threads;
	result["process_info"]["priority"] = processInfo.priority;
	result["process_info"]["start_time"] = processInfo.startTime;
	result["process_info"]["end_time"] = processInfo.endTime;
	result["process_info"]["duration"] = processInfo.duration;
	result["process_info"]["open_files"] = Json::Value(Json::arrayValue);
	for (const std::string& file : processInfo.openFiles) {
		result["process_info"]["open_files"].append(file);
	}
	result["process_info"]["modules"] = Json::Value(Json::objectValue);
	for (const auto& module : processInfo.modules) {
		result["process_info"]["modules"][module.first] = module.second;
	}
	res.set_content(result.toStyledString(), "application/json");
}

//recuperer les informations du systeme et les erreurs
void OSINTTool::handleSystemInfo(const httplib::Request& req, httplib::Response& res) {
	Json::Value result;
	result["timestamp"] = getCurrentTimestamp();

	ConsoleUtils console;

	console.getOSInfo();
	console.getHostname();
	console.getUsername();
	console.getIPAddress();
	console.getMACAddress();
	console.getCPUName();
	console.getMemoryInfo();
	console.getGPUInfo();
	console.getDriveInfo();
	console.getSystemErrors();

	res.set_content(result.toStyledString(), "application/json");

}




OSINTTool::ScriptResult OSINTTool::executePythonScript(const std::string& script)
{
	ScriptResult result;
	PyObject* mainModule = PyImport_AddModule("__main__");
	PyObject* globalDict = PyModule_GetDict(mainModule);
	PyObject* localDict = PyDict_New();
	PyDict_SetItemString(localDict, "__builtins__", PyEval_GetBuiltins());
	PyObject* compiledCode = Py_CompileString(script.c_str(), "<string>", Py_file_input);
	if (compiledCode) {
		result.success = true;
		PyObject* resultObj;
		if (resultObj) {
			if (PyUnicode_Check(resultObj)) {
				result.output = PyUnicode_AsUTF8(resultObj);
			}
			Py_DECREF(resultObj);
		}
		else {
			result.error = "Error executing script";
		}
	}
	return result;
}

OSINTTool::ScriptResult OSINTTool::executePythonFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return {false, "", "Unable to open file: " + filename};
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return executePythonScript(buffer.str());
}

OSINTTool::ScriptResult OSINTTool::executeLuaScript(const std::string& script) {
    ScriptResult result;
    
    try {
        if (luaL_dostring(luaState, script.c_str()) != LUA_OK) {
            result.success = false;
            result.error = lua_tostring(luaState, -1);
            lua_pop(luaState, 1);
        } else {
            result.success = true;
            if (lua_isstring(luaState, -1)) {
                result.output = lua_tostring(luaState, -1);
                lua_pop(luaState, 1);
            }
        }
    }
    catch (const std::exception& e) {
        result.success = false;
        result.error = e.what();
    }
    
    return result;
}

OSINTTool::ScriptResult OSINTTool::executeLuaFile(const std::string& filename) {
    ScriptResult result;
    
    try {
        if (luaL_dofile(luaState, filename.c_str()) != LUA_OK) {
            result.success = false;
            result.error = lua_tostring(luaState, -1);
            lua_pop(luaState, 1);
        } else {
            result.success = true;
            if (lua_isstring(luaState, -1)) {
                result.output = lua_tostring(luaState, -1);
                lua_pop(luaState, 1);
            }
        }
    }
    catch (const std::exception& e) {
        result.success = false;
        result.error = e.what();
    }
    
    return result;
}

void OSINTTool::handleScriptExecution(const httplib::Request& req, httplib::Response& res) {
    try {
        Json::Value result;
        result["timestamp"] = getCurrentTimestamp();
        
        std::string scriptType = req.get_param_value("type");
        std::string scriptContent = req.get_param_value("script");
        std::string filename = req.get_param_value("filename");
        
        ScriptResult scriptResult;
        
        if (scriptType == "python") {
            if (!filename.empty()) {
                scriptResult = executePythonFile(scriptPath + "\\" + filename);
            } else {
                scriptResult = executePythonScript(scriptContent);
            }
        } else if (scriptType == "lua") {
            if (!filename.empty()) {
                scriptResult = executeLuaFile(scriptPath + "\\" + filename);
            } else {
                scriptResult = executeLuaScript(scriptContent);
            }
        } else {
            handleError(res, "Invalid script type", 400);
            return;
        }
        
        result["success"] = scriptResult.success;
        result["output"] = scriptResult.output;
        result["error"] = scriptResult.error;
        
        res.set_content(result.toStyledString(), "application/json");
    }
    catch (const std::exception& e) {
        handleError(res, e.what(), 500);
    }
}

void OSINTTool::handleError(httplib::Response& res, const std::string& message, int code)
{
	Json::Value result;
	result["timestamp"] = getCurrentTimestamp();
	result["success"] = false;
	result["error"] = message;
	res.set_content(result.toStyledString(), "application/json");
	res.status = code;

}

bool OSINTTool::startWebInterface(int port) {
    try {
        if (webInterfaceActive) {
            std::cout << "Web interface is already running.\n";
            return true;
        }

        webServer = std::make_unique<WebServer>(port);
        webServer->setStaticDir("web/static");
        
        // Configuration des routes API
        webServer->addRoute("/api/network/test", 
            [this](const httplib::Request& req, httplib::Response& res) {
                handleNetworkTest(req, res);
            });
        
        webServer->addRoute("/api/process/analyze", 
            [this](const httplib::Request& req, httplib::Response& res) {
                handleProcessAnalysis(req, res);
            });
        
        webServer->addRoute("/api/system/info", 
            [this](const httplib::Request& req, httplib::Response& res) {
                handleSystemInfo(req, res);
            });
            
        webServer->addRoute("/api/script/execute", 
            [this](const httplib::Request& req, httplib::Response& res) {
                handleScriptExecution(req, res);
            });

        if (!webServer->start()) {
            std::cerr << "Failed to start web interface\n";
            return false;
        }

        webInterfaceActive = true;
        std::cout << "Web interface started at: " << webServer->getPublicUrl() << "\n";
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error starting web interface: " << e.what() << "\n";
        return false;
    }
}

void OSINTTool::stopWebInterface()
{
}

std::string OSINTTool::getWebUrl() const
{
    return std::string();
}

HRESULT SHGetFolderPathA(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath)
{
	// Fonction factice pour éviter les erreurs de compilation avec SHGetFolderPathA sur Windows 10 et versions ultérieures 
	return 0;

}

std::string OSINTTool::NetworkInfo::resolveDNS(const std::string& hostname)
{
	return std::string();
}

std::vector<int> OSINTTool::NetworkInfo::scanPorts(const std::string& ip, int startPort, int endPort)
{
	return std::vector<int>();
}

bool OSINTTool::NetworkInfo::testConnection(const std::string& host, int port)
{
	return false;
}

std::string OSINTTool::NetworkInfo::getMACAddress(const std::string& ip)
{
	return std::string();
}

std::string OSINTTool::NetworkInfo::getSubnetMask(const std::string& ip)
{
	return std::string();
}

std::string OSINTTool::NetworkInfo::getDefaultGateway(const std::string& ip)
{
	return std::string();
}

std::vector<std::string> OSINTTool::NetworkInfo::getDNSServers(const std::string& ip)
{
	return std::vector<std::string>();
}

std::map<std::string, std::string> OSINTTool::ProcessInfo::getModules(DWORD pid)
{
	return std::map<std::string, std::string>();
}

std::vector<std::string> OSINTTool::ProcessInfo::getOpenFiles(DWORD pid)
{
	return std::vector<std::string>();
}

std::string OSINTTool::ProcessInfo::getOwner(DWORD pid)
{
	return std::string();
}

std::string OSINTTool::ProcessInfo::getStartTime(DWORD pid)
{
	return std::string();
}

std::string OSINTTool::ProcessInfo::getEndTime(DWORD pid)
{
	return std::string();
}

std::string OSINTTool::ProcessInfo::getDuration(DWORD pid)
{
	return std::string();
}

int OSINTTool::ProcessInfo::getPriority(DWORD pid)
{
	return 0;
}

int OSINTTool::ProcessInfo::getThreadCount(DWORD pid)
{
	return 0;
}

std::string OSINTTool::ProcessInfo::getCommandLine(DWORD pid)
{
	return std::string();
}

std::string OSINTTool::ProcessInfo::getPath(DWORD pid)
{
	return std::string();
}

std::string OSINTTool::ProcessInfo::getName(DWORD pid)
{
	return std::string();
}

DWORD OSINTTool::ProcessInfo::getParentPid(DWORD pid)
{
	return 0;
}

std::string OSINTTool::ProcessInfo::getError()
{
	return std::string();
}

OSINTTool::ProcessInfo OSINTTool::ProcessInfo::analyzeProcess(const std::string& name)
{
	return ProcessInfo();
}

std::vector<OSINTTool::ProcessInfo> OSINTTool::ProcessInfo::listProcesses()
{
	return std::vector<ProcessInfo>();
}

bool OSINTTool::ProcessInfo::killProcess(DWORD pid)
{
	return false;
}

std::map<std::string, std::string> OSINTTool::ProcessInfo::getProcessModules(DWORD pid)
{
	return std::map<std::string, std::string>();
}
