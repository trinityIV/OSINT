#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <chrono>
#include <thread>
#include <random>
#include <array>

class AsciiAnimation {
private:
    const std::vector<std::string> cyberpunkLogo = {
        "╔══════════════════════════════════════════════════════════╗",
        "║ ░█▀▀ █░█ █▄▄ █▀▀ █▀█ █▀█ █░█ █▄░█ █▄▀  █▀█ █▀ █ █▄░█ ║",
        "║ ░█▄▄ █▄█ █▄█ ██▄ █▀▄ █▀▀ █▄█ █░▀█ █░█  █▄█ ▄█ █ █░▀█ ║",
        "╠══════════════════════════════════════════════════════════╣",
        "║     [ERROR 404: REALITY NOT FOUND]                       ║",
        "║     <SYSTEM://BREACH_PROTOCOL_INITIATED>                 ║",
        "║     {NEURAL.LINK::ESTABLISHED}                          ║",
        "║     |MATRIX.ACCESS.GRANTED|                             ║",
        "╠══════════════════════════════════════════════════════════╣",
        "║ █▀█ █▀ █ █▄░█ ▀█▀   ▀█▀ █▀█ █▀█ █░░   █▀ █░█ █ ▀█▀ █▀▀║",
        "║ █▄█ ▄█ █ █░▀█ ░█░   ░█░ █▄█ █▄█ █▄▄   ▄█ █▄█ █ ░█░ ██▄║",
        "╚══════════════════════════════════════════════════════════╝"
    };

    // Matrix-style caractères pour l'effet de pluie numérique
    const std::string matrixChars = "ﾊﾐﾋｰｳｼﾅﾓﾆｻﾜﾂｵﾘｱﾎﾃﾏｹﾒｴｶｷﾑﾕﾗｾﾈｽﾀﾇﾍ";

    // Couleurs cyberpunk améliorées
    const std::vector<std::pair<WORD, WORD>> cyberpunkColors = {
        {FOREGROUND_RED | FOREGROUND_INTENSITY, 
         BACKGROUND_RED | BACKGROUND_INTENSITY}, // Rouge néon
        {FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
         BACKGROUND_GREEN | BACKGROUND_BLUE}, // Cyan électrique
        {FOREGROUND_RED | FOREGROUND_GREEN,
         BACKGROUND_RED | BACKGROUND_GREEN}, // Or cybernétique
        {FOREGROUND_BLUE | FOREGROUND_INTENSITY,
         BACKGROUND_BLUE}, // Bleu plasma
        {FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
         BACKGROUND_RED | BACKGROUND_BLUE}, // Magenta synthwave
        {FOREGROUND_GREEN | FOREGROUND_INTENSITY,
         BACKGROUND_GREEN} // Vert matrice
    };

    HANDLE consoleHandle;
    std::mt19937 rng;
    int currentFrame;
    bool isAnimating;
    std::thread animationThread;
    std::vector<int> rainDrops;

    void setConsoleColor(const std::pair<WORD, WORD>& colors, bool glitch = false) {
        if (glitch) {
            SetConsoleTextAttribute(consoleHandle, colors.second);
        } else {
            SetConsoleTextAttribute(consoleHandle, colors.first);
        }
    }

    void resetConsoleColor() {
        SetConsoleTextAttribute(consoleHandle, 
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    void clearScreen() {
        COORD topLeft = { 0, 0 };
        CONSOLE_SCREEN_BUFFER_INFO screen;
        DWORD written;

        GetConsoleScreenBufferInfo(consoleHandle, &screen);
        FillConsoleOutputCharacterA(consoleHandle, ' ', 
            screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
        FillConsoleOutputAttribute(consoleHandle, 
            FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
            screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
        SetConsoleCursorPosition(consoleHandle, topLeft);
    }

    void drawMatrixRain() {
        CONSOLE_SCREEN_BUFFER_INFO screen;
        GetConsoleScreenBufferInfo(consoleHandle, &screen);
        int width = screen.dwSize.X;
        
        // Initialiser les gouttes de pluie si nécessaire
        if (rainDrops.empty()) {
            rainDrops.resize(width, 0);
        }

        // Mettre à jour et dessiner chaque goutte
        for (int x = 0; x < width; x++) {
            if (rainDrops[x] > 0) {
                COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(rainDrops[x] - 1) };
                SetConsoleCursorPosition(consoleHandle, pos);
                
                // Dégradé de couleur pour l'effet de traînée
                setConsoleColor(cyberpunkColors[2], false); // Vert plus clair pour la tête
                std::cout << matrixChars[rand() % matrixChars.length()];
                
                if (rand() % 100 < 30) { // 30% de chance de faire avancer la goutte
                    rainDrops[x]++;
                }
                
                // Réinitialiser si la goutte atteint le bas
                if (rainDrops[x] > screen.dwSize.Y) {
                    rainDrops[x] = 0;
                }
            } else if (rand() % 100 < 5) { // 5% de chance de créer une nouvelle goutte
                rainDrops[x] = 1;
            }
        }
    }

    void drawGlitchEffect() {
        std::uniform_int_distribution<> colorDist(0, cyberpunkColors.size() - 1);
        std::uniform_int_distribution<> lineDist(0, cyberpunkLogo.size() - 1);
        std::uniform_int_distribution<> charDist(0, matrixChars.length() - 1);
        
        for (int i = 0; i < 5; ++i) {
            int line = lineDist(rng);
            auto& color = cyberpunkColors[colorDist(rng)];
            setConsoleColor(color, true);
            
            COORD pos = { 0, static_cast<SHORT>(line) };
            SetConsoleCursorPosition(consoleHandle, pos);
            
            std::string glitchedLine = cyberpunkLogo[line];
            // Ajouter des caractères matrix aléatoires
            for (int j = 0; j < 3; ++j) {
                int pos = rand() % glitchedLine.length();
                glitchedLine[pos] = matrixChars[charDist(rng)];
            }
            std::cout << glitchedLine << std::endl;
        }
    }

    void animationLoop() {
        while (isAnimating) {
            clearScreen();
            
            // Effet de pluie matrix en arrière-plan
            drawMatrixRain();
            
            // Dessiner le logo avec des couleurs changeantes
            for (size_t i = 0; i < cyberpunkLogo.size(); ++i) {
                auto& color = cyberpunkColors[(currentFrame + i) % cyberpunkColors.size()];
                setConsoleColor(color, (rand() % 100 < 5)); // 5% chance de glitch
                COORD pos = { 0, static_cast<SHORT>(i + 2) }; // Décalage pour la pluie
                SetConsoleCursorPosition(consoleHandle, pos);
                std::cout << cyberpunkLogo[i] << std::endl;
            }

            // Effets de glitch plus fréquents et intenses
            if (currentFrame % 3 == 0) {
                drawGlitchEffect();
            }

            currentFrame = (currentFrame + 1) % 1000;
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Animation plus rapide
        }
    }

public:
    AsciiAnimation() : 
        consoleHandle(GetStdHandle(STD_OUTPUT_HANDLE)),
        rng(std::random_device{}()),
        currentFrame(0),
        isAnimating(false) {
        // Configurer la console pour l'animation
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(consoleHandle, &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(consoleHandle, &cursorInfo);

        // Maximiser la fenêtre de console
        ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
    }

    ~AsciiAnimation() {
        stop();
    }

    void start() {
        if (!isAnimating) {
            isAnimating = true;
            animationThread = std::thread(&AsciiAnimation::animationLoop, this);
        }
    }

    void stop() {
        if (isAnimating) {
            isAnimating = false;
            if (animationThread.joinable()) {
                animationThread.join();
            }
            resetConsoleColor();
            clearScreen();
        }
    }

    void displayStaticLogo() {
        clearScreen();
        drawMatrixRain(); // Ajouter l'effet de pluie en arrière-plan
        
        for (const auto& line : cyberpunkLogo) {
            auto& color = cyberpunkColors[currentFrame % cyberpunkColors.size()];
            setConsoleColor(color, false);
            std::cout << line << std::endl;
            currentFrame++;
        }
        resetConsoleColor();
    }
};
