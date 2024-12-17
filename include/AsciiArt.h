#pragma once
#include <vector>
#include <string>

class AsciiArt {
public:
    AsciiArt();
    std::vector<std::string> getLogoFrames() const;
    std::string getMainLogo() const;
	void printLogo() const;
	void printLogoFrame(int frame) const;
	void printMainLogo() const;

private:
    std::vector<std::string> logoFrames;
    std::string mainLogo;
    void initializeFrames();
    void initializeMainLogo();

};