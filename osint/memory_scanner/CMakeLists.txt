cmake_minimum_required(VERSION 3.15)
project(OSINT_Memory_Scanner)

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

# Sources
set(SOURCES
    main.cpp
)

# Créer l'exécutable
add_executable(memory_scanner ${SOURCES})

# Lier les bibliothèques
target_link_libraries(memory_scanner PRIVATE
    nlohmann_json::nlohmann_json
)

# Ajouter les définitions Windows nécessaires
if(WIN32)
    target_compile_definitions(memory_scanner PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
    )
endif()
