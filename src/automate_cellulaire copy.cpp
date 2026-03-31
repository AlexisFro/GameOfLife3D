#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

class Cell {
private:
    int m_x, m_y, m_z;
    bool m_is_alive = false;
    int m_neighbors = 0;

public:
    Cell() : m_x(0), m_y(0), m_z(0) {}
    Cell(int x, int y, int z) : m_x(x), m_y(y), m_z(z) {}

    void update_neighbors(int neighbors) { m_neighbors = neighbors; }
    void update_is_alive(bool alive) { m_is_alive = alive; }
    
    int getNeighbors() const { return m_neighbors; }
    bool getIsAlive() const { return m_is_alive; }
    int x() const { return m_x; }
    int y() const { return m_y; }
    int z() const { return m_z; }
};

class Board3D {
private:
    int m_size; // On part sur un cube de taille N x N x N
    std::vector<Cell> m_cells;

public:
    Board3D(int size) : m_size(size) {


        int size_total = m_size * m_size * m_size;
        m_cells.resize(size_total);        
        for(int i=0; i<size_total; ++i){

            int x = i % m_size;
            int y = (i / m_size) % m_size;
            int z = i / (m_size * m_size);

            m_cells[i] = Cell{x, y ,z};
        }
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

    void step() {
        // 1. Calculer le nombre de voisins pour tout le monde
        for(int i = 0; i < m_size; ++i)
            for(int j = 0; j < m_size; ++j)
                for(int k = 0; k < m_size; ++k)
                    m_cells[i][j][k].update_neighbors(count_neighbors(i, j, k));
        for(int i = 0; i < m_size; ++i) {
            for(int j = 0; j < m_size; ++j) {
                for(int k = 0; k < m_size; ++k) {
                    Cell& c = m_cells[i][j][k];
                    int n = c.getNeighbors();
                    
                    if(c.getIsAlive()) {
                        if(n < 5 || n > 8) c.update_is_alive(false);
                    } else {
                        if(n == 6) c.update_is_alive(true);
                    }
                }
            }
        }
    }

    bool is_alive(int x, int y, int z) {
    if (x < 0 || x >= m_size || y < 0 || y >= m_size || z < 0 || z >= m_size) return false;
    
    return m_cells[x + (y * m_size) + (z * m_size * m_size)].getIsAlive();
}

    bool is_alive(int x, int y, int z) const {
        return m_cells[x][y][z].getIsAlive();
    }
};