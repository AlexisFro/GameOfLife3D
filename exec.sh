#!/bin/bash

# Nom de ton exécutable (doit être le même que dans CMakeLists.txt)
EXEC="mon_programme"

echo "🔨 Début de la compilation..."

# 1. Crée le dossier build s'il n'existe pas (-p évite les erreurs s'il est déjà là)
mkdir -p build

# 2. Va dans le dossier build
cd build

# 3. Prépare les fichiers avec CMake
cmake ..

# 4. Compile le code
# L'astuce "-j$(nproc)" permet d'utiliser tous les cœurs de ton processeur 
# pour compiler beaucoup plus vite !
make -j$(nproc)

# 5. Vérifie si la compilation a réussi
if [ $? -eq 0 ]; then
    echo "✅ Compilation réussie !"
    cd ..
    echo "🚀 Lancement de $EXEC..."
    echo "----------------------------------------"
    
    # --- LA MAGIE EST ICI ---
    # Le $@ transfère tes arguments (1 ou 2) au programme C++
    ./build/$EXEC $@
else
    echo "❌ Erreur de compilation. Le programme n'a pas été lancé."
fi