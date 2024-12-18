#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iomanip>

using json = nlohmann::json;

struct FunctionPattern {
    std::string name;
    std::vector<BYTE> pattern;
    LPCVOID address;
};

struct ScanResult {
    std::string functionName;
    LPCVOID address;
    bool found;
    std::vector<BYTE> pattern;
};

std::vector<ScanResult> scanResults;

void displayResults() {
    std::cout << "\n=== Résultats du Scan ===\n" << std::endl;
    std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
    std::cout << std::setfill(' ');
    std::cout << std::left << std::setw(20) << "Fonction"
              << std::setw(20) << "Statut"
              << std::setw(20) << "Adresse"
              << "Pattern" << std::endl;
    std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
    std::cout << std::setfill(' ');

    for (const auto& result : scanResults) {
        std::cout << std::left << std::setw(20) << result.functionName
                  << std::setw(20) << (result.found ? "Trouvée" : "Non trouvée")
                  << std::setw(20) << (result.found ? "0x" + std::to_string((uintptr_t)result.address) : "N/A");
        
        // Afficher le pattern en hexadécimal
        if (!result.pattern.empty()) {
            std::cout << std::hex << std::uppercase;
            for (const auto& byte : result.pattern) {
                std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
            }
            std::cout << std::dec;
        } else {
            std::cout << "N/A";
        }
        std::cout << std::endl;
    }
    std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
}

// Function to read a specific pattern from a process's memory
std::pair<bool, LPCVOID> readMemoryPattern(HANDLE processHandle, LPCVOID baseAddress, const std::vector<BYTE>& pattern) {
    BYTE buffer[256];
    SIZE_T bytesRead;
    if (ReadProcessMemory(processHandle, baseAddress, buffer, sizeof(buffer), &bytesRead)) {
        for (size_t i = 0; i < bytesRead - pattern.size(); ++i) {
            bool found = true;
            for (size_t j = 0; j < pattern.size(); ++j) {
                if (buffer[i + j] != pattern[j]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                LPCVOID foundAddress = (LPCVOID)((BYTE*)baseAddress + i);
                std::cout << "Pattern found at address: 0x" << std::hex << (uintptr_t)foundAddress << std::dec << std::endl;
                return {true, foundAddress};
            }
        }
    }
    return {false, nullptr};
}

// Function to simulate finding a function by name and generate a pattern
FunctionPattern findFunctionByName(const std::string& functionName) {
    // Simulated mapping of function names to patterns and addresses
    std::unordered_map<std::string, std::pair<std::vector<BYTE>, LPCVOID>> functionMap = {
        {"m_iHealth", {{0x89, 0x48, 0x04}, (LPCVOID)0x1000}},
        {"m_iArmor", {{0x89, 0x48, 0x08}, (LPCVOID)0x2000}},
        {"m_iAmmo", {{0x89, 0x48, 0x0C}, (LPCVOID)0x3000}}
    };

    auto it = functionMap.find(functionName);
    if (it != functionMap.end()) {
        std::cout << "Function " << functionName << " pattern generated at address: 0x" 
                  << std::hex << (uintptr_t)(it->second.second) << std::dec << std::endl;
        return {functionName, it->second.first, it->second.second};
    }
    return {functionName, std::vector<BYTE>(), nullptr};
}

void scanProcess(DWORD processID, const FunctionPattern& pattern) {
    ScanResult result;
    result.functionName = pattern.name;
    result.pattern = pattern.pattern;
    result.found = false;
    result.address = nullptr;

    if (pattern.pattern.empty()) {
        std::cerr << "No pattern available for function: " << pattern.name << std::endl;
        scanResults.push_back(result);
        return;
    }

    HANDLE processHandle = OpenProcess(PROCESS_VM_READ, FALSE, processID);
    if (processHandle) {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        LPCVOID address = sysInfo.lpMinimumApplicationAddress;
        
        std::cout << "Scanning for function: " << pattern.name << std::endl;
        
        while (address < sysInfo.lpMaximumApplicationAddress) {
            auto [found, foundAddress] = readMemoryPattern(processHandle, address, pattern.pattern);
            if (found) {
                result.found = true;
                result.address = foundAddress;
                break;
            }
            address = (LPCVOID)((BYTE*)address + 256);
        }
        CloseHandle(processHandle);
    } else {
        std::cerr << "Failed to open process with ID: " << processID << std::endl;
    }
    
    scanResults.push_back(result);
}

void processFunctionNames(DWORD processID, const std::vector<std::string>& functionNames) {
    for (const auto& name : functionNames) {
        auto pattern = findFunctionByName(name);
        scanProcess(processID, pattern);
    }
}

void readFunctionNamesFromJson(DWORD processID, const std::string& filePath) {
    try {
        std::ifstream inputFile(filePath);
        if (!inputFile.is_open()) {
            throw std::runtime_error("Failed to open JSON file: " + filePath);
        }

        json jsonData = json::parse(inputFile);
        
        if (jsonData.contains("functions") && jsonData["functions"].is_array()) {
            std::vector<std::string> functionNames = jsonData["functions"].get<std::vector<std::string>>();
            processFunctionNames(processID, functionNames);
        } else {
            throw std::runtime_error("Invalid JSON format: 'functions' array not found.");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    DWORD processID;
    std::string jsonFilePath;

    std::cout << "Enter target process ID: ";
    std::cin >> processID;

    std::cout << "Enter path to functions JSON file: ";
    std::cin >> jsonFilePath;

    scanResults.clear(); // Réinitialiser les résultats
    readFunctionNamesFromJson(processID, jsonFilePath);
    
    // Afficher les résultats à la fin
    displayResults();

    return 0;
}
