@echo off
echo Building OSINT Tool Suite...

REM Créer le dossier build s'il n'existe pas
if not exist "build" mkdir build

REM Aller dans le dossier build
cd build

REM Configurer avec CMake
cmake -G "Visual Studio 17 2022" -A x64 ..

REM Compiler le projet
cmake --build . --config Release

REM Copier l'exécutable dans le dossier principal
copy /Y Release\osint_suite.exe ..\osint_suite.exe

echo Build complete!
pause
