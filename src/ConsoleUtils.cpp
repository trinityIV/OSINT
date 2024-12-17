#include "ConsoleUtils.h"
#include <iostream>
#include <windows.h>
#include <Iphlpapi.h>

void ConsoleUtils::getOSInfo()
{
	// Get the OS information
	OSVERSIONINFOEX osInfo;
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);
	GetVersionExA((LPOSVERSIONINFOA)&osInfo);
	std::cout << "OS version: " << osInfo.dwMajorVersion << "." << osInfo.dwMinorVersion << "\n";
	std::cout << "OS platform ID: " << osInfo.dwPlatformId << "\n";
	std::cout << "OS build number: " << osInfo.dwBuildNumber << "\n";
	std::cout << "OS service pack: " << osInfo.szCSDVersion << "\n";

}

void ConsoleUtils::getHostname()
{
	// Get the hostname
	char hostname[256];
	DWORD size = sizeof(hostname);
	GetComputerNameA(hostname, &size);
	std::cout << "Hostname: " << hostname << "\n";
}

//fonction username
void ConsoleUtils::getUsername() {
	// Get the username
	char username[UNLEN + 1];
	DWORD size = UNLEN + 1;
	GetUserNameA(username, &size);
	std::cout << "Username: " << username << "\n";
}



void ConsoleUtils::getIPAddress()
{
	// Get the IP address
	PIP_ADAPTER_INFO& adapterInfo = new IP_ADAPTER_INFO[16];
	DWORD size = sizeof(adapterInfo);
	DWORD result = GetAdaptersInfo(adapterInfo, &size);
	if (result == ERROR_SUCCESS) {
		PIP_ADAPTER_INFO adapter = adapterInfo;
		while (adapter) {
			IP_ADDR_STRING* ipAddress = &adapter->IpAddressList;
			while (ipAddress) {
				std::cout << "Adapter name: " << adapter->AdapterName << "\n";
				std::cout << "IP address: " << ipAddress->IpAddress.String << "\n";
				ipAddress = ipAddress->Next;
			}
			adapter = adapter->Next;
		}
	}
	else {
		std::cout << "Failed to get IP address\n";
	}

}

void ConsoleUtils::getMACAddress()
{
	// Get the MAC address
	IP_ADAPTER_INFO adapterInfo[16];
	DWORD size = sizeof(adapterInfo);
	DWORD result = GetAdaptersInfo(adapterInfo, &size);
	if (result == ERROR_SUCCESS) {
		PIP_ADAPTER_INFO adapter = adapterInfo;
		while (adapter) {
			std::cout << "Adapter name: " << adapter->AdapterName << "\n";
			std::cout << "MAC address: ";
			for (int i = 0; i < adapter->AddressLength; ++i) {
				printf("%02X", adapter->Address[i]);
				if (i < adapter->AddressLength - 1) {
					std::cout << "-";
				}
			}
			std::cout << "\n";
			adapter = adapter->Next;
		}
	}
	else {
		std::cout << "Failed to get MAC address\n";
	}

}

void ConsoleUtils::getCPUName()
{
	// Get the CPU information
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	std::cout << "Number of processors: " << systemInfo.dwNumberOfProcessors << "\n";
	std::cout << "Processor type: " << systemInfo.dwProcessorType << "\n";
	std::cout << "Processor level: " << systemInfo.wProcessorLevel << "\n";
	std::cout << "Processor revision: " << systemInfo.wProcessorRevision << "\n";

}

void ConsoleUtils::getMemoryInfo()
{
	// Get the memory information
	MEMORYSTATUSEX memoryStatus;
	memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memoryStatus);
	std::cout << "Memory load: " << memoryStatus.dwMemoryLoad << "%\n";
	std::cout << "Total physical memory: " << memoryStatus.ullTotalPhys / 1024 / 1024 << " MB\n";
	std::cout << "Free physical memory: " << memoryStatus.ullAvailPhys / 1024 / 1024 << " MB\n";
	std::cout << "Total virtual memory: " << memoryStatus.ullTotalVirtual / 1024 / 1024 << " MB\n";
	std::cout << "Free virtual memory: " << memoryStatus.ullAvailVirtual / 1024 / 1024 << " MB\n";
	std::cout << "Total page file: " << memoryStatus.ullTotalPageFile / 1024 / 1024 << " MB\n";
	std::cout << "Free page file: " << memoryStatus.ullAvailPageFile / 1024 / 1024 << " MB\n";
	std::cout << "Total extended memory: " << memoryStatus.ullTotalPhys / 1024 / 1024 << " MB\n";
	std::cout << "Free extended memory: " << memoryStatus.ullAvailExtendedVirtual / 1024 / 1024 << " MB\n";

}

void ConsoleUtils::getGPUInfo()
{
	// Get the GPU information
	DISPLAY_DEVICE displayDevice;
	displayDevice.cb = sizeof(DISPLAY_DEVICE);
	DWORD deviceNum = 0;
	while (EnumDisplayDevices(NULL, deviceNum, &displayDevice, 0)) {
		std::cout << "Device name: " << displayDevice.DeviceName << "\n";
		std::cout << "Device string: " << displayDevice.DeviceString << "\n";
		std::cout << "State: " << (displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE ? "Active" : "Inactive") << "\n";
		std::cout << "Primary: " << (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ? "Yes" : "No") << "\n";
		++deviceNum;
	}

}

void ConsoleUtils::getDriveInfo()
{
	// Get the logical drives
	DWORD drives = GetLogicalDrives();
	std::cout << "Logical drives: ";
	for (int i = 0; i < 26; ++i) {
		if (drives & (1 << i)) {
			std::cout << static_cast<char>('A' + i) << " ";
		}
	}
	std::cout << "\n";

}

void ConsoleUtils::getSystemErrors()
{
	// Get the last error message
	DWORD errorCode = GetLastError();
	if (errorCode != 0) {
		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
		std::string message(messageBuffer, size);
		LocalFree(messageBuffer);
		std::cout << "Error code: " << errorCode << "\n";
		std::cout << "Error message: " << message << "\n";
	}
	else {
		std::cout << "No error occurred\n";
	}

}

ConsoleUtils::ConsoleUtils() {
    // Initialisation de la console
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void ConsoleUtils::setTextColor(ConsoleColor color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void ConsoleUtils::clearScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void ConsoleUtils::gotoxy(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(hConsole, pos);
}

void ConsoleUtils::showCursor(bool visible) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void ConsoleUtils::resetTextColor()
{
	setTextColor(COLOR_WHITE);
}
