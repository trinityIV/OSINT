cmake_minimum_required(VERSION 3.15)
project(OSINT_Web_Server)

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

# Sources
file(GLOB SOURCES "*.cpp")

# Créer l'exécutable
add_executable(web_server ${SOURCES})

# Lier les bibliothèques
target_link_libraries(web_server PRIVATE
    nlohmann_json::nlohmann_json
    CURL::libcurl
)

# Ajouter les définitions Windows nécessaires
if(WIN32)
    target_compile_definitions(web_server PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
    )
endif()

# Copier les fichiers statiques
file(COPY ${CMAKE_SOURCE_DIR}/public DESTINATION ${CMAKE_BINARY_DIR})
