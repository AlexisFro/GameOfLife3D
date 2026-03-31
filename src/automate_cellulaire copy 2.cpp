#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib> // Nécessaire pour rand()

// Inclusion de GLM pour la physique spatiale
#include "../libs/glm/glm.hpp"

class Cell {
private:
    int m_x, m_y, m_z;
    bool m_is_alive = false;
    int m_neighbors = 0;
    int m_age = 0; // Nécessaire pour l'évolution de la galaxie

public:
    Cell() : m_x(0), m_y(0), m_z(0) {}
    Cell(int x, int y, int z) : m_x(x), m_y(y), m_z(z) {}

    void update_neighbors(int neighbors) { m_neighbors = neighbors; }
    void update_is_alive(bool alive) { m_is_alive = alive; }
    void set_age(int age) { m_age = age; } // Setter pour l'âge
    
    int getNeighbors() const { return m_neighbors; }
    bool getIsAlive() const { return m_is_alive; }
    int get_age() const { return m_age; } // Getter pour l'âge
    
    // Fonction utile pour la physique
    glm::vec3 getPosition() const { return glm::vec3(m_x, m_y, m_z); }
    
    int x() const { return m_x; }
    int y() const { return m_y; }
    int z() const { return m_z; }
};

class Board3D {
private:
    int m_size; // On part sur un cube de taille N x N x N
    std::vector<std::vector<std::vector<Cell>>> m_cells;

public:
    Board3D(int size) : m_size(size) {
        m_cells.resize(size, std::vector<std::vector<Cell>>(size, std::vector<Cell>(size)));
        for(int i = 0; i < size; ++i)
            for(int j = 0; j < size; ++j)
                for(int k = 0; k < size; ++k)
                    m_cells[i][j][k] = Cell{i, j, k};
    }

    // Compte les 26 voisins autour d'une cellule
    int count_neighbors(int x, int y, int z) {
        int total = 0;
        for(int i = x - 1; i <= x + 1; ++i) {
            for(int j = y - 1; j <= y + 1; ++j) {
                for(int k = z - 1; k <= z + 1; ++k) {
                    // On vérifie les limites et on exclut la cellule centrale
                    if(i >= 0 && i < m_size && j >= 0 && j < m_size && k >= 0 && k < m_size) {
                        if(!(i == x && j == y && k == z)) {
                            if(m_cells[i][j][k].getIsAlive()) total++;
                        }
                    }
                }
            }
        }
        return total;
    }

    // --- LA NOUVELLE PHYSIQUE DE GALAXIE EN 3D ---
    void step() {
        std::vector<std::vector<std::vector<Cell>>> next_state = m_cells; 
        float centre_f = (m_size - 1) / 2.0f;
        glm::vec3 centre_v = glm::vec3(centre_f, centre_f, centre_f);

        // TRIPLE BOUCLE pour parcourir ton tableau 3D
        for (int x = 0; x < m_size; ++x) {
            for (int y = 0; y < m_size; ++y) {
                for (int z = 0; z < m_size; ++z) {
                    
                    if (m_cells[x][y][z].getIsAlive()) {
                        // --- NOUVELLE PHYSIQUE DE GALAXIE ---
                        glm::vec3 pos = m_cells[x][y][z].getPosition();
                        glm::vec3 vec_vers_centre = centre_v - pos;
                        float dist = glm::length(vec_vers_centre);

                        if (dist < 1.0f) continue; // On laisse le centre tranquille

                        // 1. Vecteur directeur (vers l'extérieur au lieu de l'intérieur)
                        glm::vec3 direction_ext = -glm::normalize(vec_vers_centre);

                        // 2. Force d'expansion (Très légère pour que les bras s'écartent doucement)
                        // On ajoute un peu d'aléatoire pour un effet "poussière" organique
                        float random_push = (rand() % 100) / 100.0f * 0.5f;
                        glm::vec3 force_expansion = direction_ext * (0.2f + random_push);

                        // 3. Force de Rotation (Le Tourbillon)
                        glm::vec3 force_rotation = glm::cross(glm::vec3(0, 1, 0), direction_ext);
                        // Plus la cellule est proche du centre, plus elle tourne vite
                        float vitesse_rotation = 2.5f / (dist * 0.1f + 1.0f); 
                        force_rotation *= vitesse_rotation;

                        // 4. Déplacement sur la grille
                        glm::vec3 nouvelle_pos_f = pos + force_expansion + force_rotation;
                                                
                        int nx = glm::round(nouvelle_pos_f.x);
                        int ny = glm::round(nouvelle_pos_f.y);
                        int nz = glm::round(nouvelle_pos_f.z);

                        // 5. Règles de déplacement (Avec la correction d'auto-collision)
                        if (nx == x && ny == y && nz == z) {
                            // La particule ne bouge pas assez loin ce tour-ci, elle reste sur place
                            next_state[x][y][z].set_age(m_cells[x][y][z].get_age() + 1);
                        } 
                        else if (nx >= 0 && nx < m_size && ny >= 0 && ny < m_size && nz >= 0 && nz < m_size) {
                            // Elle bouge vers une nouvelle case valide
                            if (!m_cells[nx][ny][nz].getIsAlive()) {
                                next_state[x][y][z].update_is_alive(false); // Quitte l'ancienne case
                                next_state[nx][ny][nz].update_is_alive(true); // Arrive sur la nouvelle
                                next_state[nx][ny][nz].set_age(m_cells[x][y][z].get_age() + 1); 
                            } else {
                                 next_state[x][y][z].update_is_alive(false); // Meurt par collision avec une AUTRE étoile
                            }
                        } 
                        else {
                            next_state[x][y][z].update_is_alive(false); // Sort des limites de la grille
                        }
                    }
                }
            }
        }

        // 6. NAISSANCE MASSIVE AU CENTRE (Le cœur de la galaxie)
        // On fait naître plusieurs étoiles à chaque frame
        for(int n = 0; n < 8; ++n) {
            int cx = centre_f + (rand() % 5 - 2);
            // On aplatit un peu la galaxie sur l'axe Y (épaisseur de 3 cases)
            int cy = centre_f + (rand() % 3 - 1); 
            int cz = centre_f + (rand() % 5 - 2);
            
            if (cx >= 0 && cx < m_size && cy >= 0 && cy < m_size && cz >= 0 && cz < m_size) {
                next_state[cx][cy][cz].update_is_alive(true);
                next_state[cx][cy][cz].set_age(0);
            }
        }

        m_cells = next_state; // Appliquer le nouvel état
    }

    void set_alive(int x, int y, int z) {
        if(x >= 0 && x < m_size && y >= 0 && y < m_size && z >= 0 && z < m_size)
            m_cells[x][y][z].update_is_alive(true);
    }

    bool is_alive(int x, int y, int z) const {
        return m_cells[x][y][z].getIsAlive();
    }
};