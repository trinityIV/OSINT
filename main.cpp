#include "OSINTTool.h"
#include <conio.h>



using namespace std;

void clearScreen() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD Position = { 0, 0 };
    SetConsoleCursorPosition(hOut, Position);
}

bool getValidInput(int& input) {
	std::string line;
	std::getline(std::cin, line);
	try {
		input = std::stoi(line);
		return true;
	}
	catch (const std::exception&) {
		return false;
	}
}


std::string getValidString() {
    std::string input;
    std::getline(std::cin, input);
    return input;
}

void showMenu() {
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

void showLoading(const std::string& message) {
	std::cout << message << " ";
	for (int i = 0; i < 3; ++i) {
		std::cout << ".";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	std::cout << "\n";
}



class Application {
public:
    Application() {
        // Configuration de la console
        SetConsoleTitle("BadHabit OSINT Tool");
        system("mode con: cols=120 lines=40");

        console.setTextColor(ConsoleUtils::COLOR_CYAN);
		std::cout << "Welcome to BadHabit OSINT Tool!\n";

    }

    void run() {
        showStartupAnimation();
        mainLoop();
    }

private:
    AsciiArt ascii;
    ConsoleUtils console;
    OSINTTool osint;

    void showStartupAnimation() {
        for (const auto& frame : ascii.getLogoFrames()) {
            clearScreen();
            std::cout << frame << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void mainLoop() {
        int choice;
        bool running = true;

        while (running) {
            showMenu();

            if (!getValidInput(choice)) {
                console.setTextColor(ConsoleUtils::COLOR_RED);
                std::cout << "Invalid input. Please enter a number.\n";
                console.resetTextColor();
                continue;
            }

            console.clearScreen();

            switch (choice) {
            case 1:
                osint.networkAnalysis();
                break;

            case 2:
                osint.domainAnalysis();
                break;

            case 3:
                osint.profileAnalysis();
                break;

            case 4:
                osint.applicationAudit();
                break;

            case 5:
                osint.generateReport();
                break;

            case 6: {
                console.setTextColor(ConsoleUtils::COLOR_CYAN);
                std::cout << "Starting web interface...\n";
                if (osint.startWebInterface()) {
                    std::cout << "Web interface started at: " << osint.getWebUrl() << "\n";
                    std::cout << "Press any key to stop the web interface...\n";
                    console.resetTextColor();
                    _getch();  // Attend une touche
                    osint.stopWebInterface();
                }
                else {
                    console.setTextColor(ConsoleUtils::COLOR_RED);
                    std::cout << "Failed to start web interface.\n";
                }
                console.resetTextColor();
                break;
            }

            case 0:
                running = false;
                break;

            default:
                console.setTextColor(ConsoleUtils::COLOR_RED);
                std::cout << "Invalid choice. Please try again.\n";
                console.resetTextColor();
                break;
            }

            if (running && choice != 6) {  // Ne pas attendre si on est dans l'interface web
                std::cout << "\nPress any key to continue...";
                _getch();
                console.clearScreen();
            }
        }
    }
};

int main() {
    try {
        Application app;
        app.run();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
