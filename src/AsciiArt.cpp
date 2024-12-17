#include "AsciiArt.h"
#include <iostream>
#include <thread>
#include <chrono>


AsciiArt::AsciiArt() {
    initializeFrames();
    initializeMainLogo();
}

void AsciiArt::initializeFrames() {
    // Frame 1
    logoFrames.push_back(R"(
    ____            _ _    _       _     _ _   
    |  _ \          | | |  | |     | |   (_) |  
    | |_) | __ _  __| | |__| | __ _| |__  _| |_ 
    |  _ < / _` |/ _` |  __  |/ _` | '_ \| | __|
    | |_) | (_| | (_| | |  | | (_| | |_) | | |_ 
    |____/ \__,_|\__,_|_|  |_|\__,_|_.__/|_|\__|
    )");

    // Frame 2
    logoFrames.push_back(R"(
     ___            _ _    _       _     _ _   
    |  _ \          | | |  | |     | |   (_) |  
    | |_) | __ _  __| | |__| | __ _| |__  _| |_ 
    |  _ < / _` |/ _` |  __  |/ _` | '_ \| | __|
    | |_) | (_| | (_| | |  | | (_| | |_) | | |_ 
    |____/ \__,_|\__,_|_|  |_|\__,_|_.__/|_|\__|
    )");

    // Frame 3
    logoFrames.push_back(R"(
    ____            _ _    _       _     _ _   
    |  _ \          | | |  | |     | |   (_) |  
    | |_) | __ _  __| | |__| | __ _| |__  _| |_ 
    |  _ < / _` |/ _` |  __  |/ _` | '_ \| | __|
    | |_) | (_| | (_| | |  | | (_| | |_) | | |_ 
    |____/ \__,_|\__,_|_|  |_|\__,_|_.__/|_|\__|
    )");
}

void AsciiArt::initializeMainLogo() {
    mainLogo = R"(
    ____            _ _    _       _     _ _   
    |  _ \          | | |  | |     | |   (_) |  
    | |_) | __ _  __| | |__| | __ _| |__  _| |_ 
    |  _ < / _` |/ _` |  __  |/ _` | '_ \| | __|
    | |_) | (_| | (_| | |  | | (_| | |_) | | |_ 
    |____/ \__,_|\__,_|_|  |_|\__,_|_.__/|_|\__|
                                    
    [OSINT Tool v1.0]
    )";
}

std::vector<std::string> AsciiArt::getLogoFrames() const {
    return logoFrames;
}

std::string AsciiArt::getMainLogo() const {
    return mainLogo;
}

void AsciiArt::printLogo() const
{
	for (const auto& frame : logoFrames) {
		std::cout << frame << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		system("cls");
	}


}

void AsciiArt::printLogoFrame(int frame) const
{
	if (frame < 0 || frame >= logoFrames.size()) {
		std::cout << "Invalid frame number" << std::endl;
		return;
	}
	std::cout << logoFrames[frame] << std::endl;

}

void AsciiArt::printMainLogo() const
{
	std::cout << mainLogo << std::endl;

}
