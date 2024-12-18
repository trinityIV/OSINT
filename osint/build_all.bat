@echo off
echo Building all OSINT projects...

REM Définir les variables d'environnement
set "VCPKG_ROOT=C:\dev\vcpkg"
set "CMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"

REM Créer le dossier build principal s'il n'existe pas
if not exist "build" mkdir build
cd build

REM Configuration et build du projet principal
echo.
echo Building main project...
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="%CMAKE_TOOLCHAIN_FILE%" ..
if %ERRORLEVEL% NEQ 0 (
    echo Error configuring main project
    exit /b 1
)

cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Error building main project
    exit /b 1
)

REM Retourner au dossier racine
cd ..

REM Build du memory scanner
echo.
echo Building memory scanner...
cd memory_scanner
if not exist "build" mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="%CMAKE_TOOLCHAIN_FILE%" ..
if %ERRORLEVEL% NEQ 0 (
    echo Error configuring memory scanner
    exit /b 1
)

cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Error building memory scanner
    exit /b 1
)

cd ../..

REM Build du web server (Node.js)
echo.
echo Building web server...
cd web_server
if not exist "node_modules" (
    echo Installing Node.js dependencies...
    npm install
)
cd ..

REM Copier tous les exécutables dans le dossier bin
if not exist "bin" mkdir bin
copy /Y "build\Release\osint_suite.exe" "bin\"
copy /Y "memory_scanner\build\Release\memory_scanner.exe" "bin\"
xcopy /Y /E /I "web_server" "bin\web_server\"

REM Copier les fichiers de configuration
copy /Y "config.json" "bin\"

echo.
echo Build complete! All executables are in the bin directory.
pause
