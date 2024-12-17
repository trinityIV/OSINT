#pragma once
#include <windows.h>
#include <string>

class ConsoleUtils {
public:
    enum ConsoleColor {
        COLOR_BLACK = 0,
        COLOR_BLUE = 1,
        COLOR_GREEN = 2,
        COLOR_CYAN = 3,
        COLOR_RED = 4,
        COLOR_MAGENTA = 5,
        COLOR_YELLOW = 6,
        COLOR_WHITE = 7
    };

	void getOSInfo();
	void getHostname();
	void getUsername();
	void getIPAddress();
	void getMACAddress();
	void getCPUName();
	void getMemoryInfo();
	void getGPUInfo();
	void getDriveInfo();
	void getSystemErrors();


    ConsoleUtils();
    void setTextColor(ConsoleColor color);
    void clearScreen();
    void gotoxy(int x, int y);
    void showCursor(bool visible);
	void resetTextColor();

};