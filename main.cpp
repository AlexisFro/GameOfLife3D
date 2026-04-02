#include "src/app.hpp"

int main(int argc, char** argv) {
    App app;
    int modeChoice = 1; // Mode par défaut (Temps réel)

    // Si on a passé un argument dans la console (ex: ./mon_programme 2)
    if (argc > 1) {
        modeChoice = std::atoi(argv[1]);
    }

    // Tu modifies ta méthode run() pour qu'elle prenne modeChoice en paramètre !
    app.initGL(modeChoice); 

    return 0;
}