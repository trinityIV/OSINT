cmake_minimum_required(VERSION 3.15)
project(OSINT_Tool_Suite)

# Définir le standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options de compilation
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Trouver les packages requis
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)

# Inclure les répertoires
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/memory_scanner)

# Sources principales
set(SOURCES
    menu.cpp
)

# Sources du memory scanner
file(GLOB MEMORY_SCANNER_SOURCES
    "${CMAKE_SOURCE_DIR}/memory_scanner/*.cpp"
)

# Créer l'exécutable principal
add_executable(osint_suite ${SOURCES})

# Créer l'exécutable du memory scanner
add_executable(memory_scanner ${CMAKE_SOURCE_DIR}/memory_scanner/main.cpp)

# Lier les bibliothèques pour l'exécutable principal
target_link_libraries(osint_suite PRIVATE
    nlohmann_json::nlohmann_json
    CURL::libcurl
)

# Lier les bibliothèques pour le memory scanner
target_link_libraries(memory_scanner PRIVATE
    nlohmann_json::nlohmann_json
)

# Ajouter les définitions Windows nécessaires
if(WIN32)
    target_compile_definitions(osint_suite PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
    )
endif()

# Copier le fichier de configuration
configure_file(${CMAKE_SOURCE_DIR}/config.json
              ${CMAKE_BINARY_DIR}/config.json COPYONLY)
