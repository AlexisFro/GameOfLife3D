# Version minimum requise de CMake
cmake_minimum_required(VERSION 3.10)

# Nom de ton projet
project(AutomateCellulaire)

# On utilise le standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# On indique au compilateur où chercher les fichiers d'en-tête (équivalent du -I.)
include_directories(${CMAKE_SOURCE_DIR})

# On crée l'exécutable en listant tous les fichiers sources
add_executable(mon_programme 
    main.cpp 
    src/app.cpp 
    libs/glad.c
    src/cube.hpp
)

# On lie les bibliothèques externes (équivalent de -lglfw -lGL -ldl)
target_link_libraries(mon_programme glfw GL dl)