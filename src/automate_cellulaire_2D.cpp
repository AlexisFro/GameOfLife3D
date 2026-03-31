#include <iostream>

#include <vector>

#include <string>

#include <thread>

#include <chrono>



class Cell{

private:

int m_x, m_y;

bool m_is_alive = false;

int m_neighbors = 0;



public:

Cell() : m_x(0), m_y(0) {}

Cell(int x, int y) : m_x(x), m_y(y) {}



void update_neighbors(int neighbors){ m_neighbors = neighbors; }

void update_is_alive(bool alive){m_is_alive = alive;}

int getNeighbors() const { return m_neighbors; }

bool getIsAlive() const { return m_is_alive; }

int x() const{return m_x;}

int y() const{return m_y;}

void x(int x){m_x = x;}

void y(int y){m_y = y;}





};



class board{

private:

int m_width, m_height;

std::vector<std::vector<Cell>> m_cells;

std::string alive = "█";

std::string dead = " ";



public:

board(int width, int height)

: m_width(width), m_height(height),

m_cells(width, std::vector<Cell>(height)) {



for(int i = 0; i < m_width; ++i){

for(int j = 0; j < m_height; ++j){

m_cells[i][j] = Cell{i,j};

}

}

}



void update_nb_neighbors(Cell & cell){

int total = 0;

for(int i = cell.x() - 1; i <= cell.x() + 1; ++i){

for(int j = cell.y() - 1; j <= cell.y() + 1; ++j){



if(i >= 0 && i < m_width && j >= 0 && j < m_height){

if(i != cell.x() || j != cell.y()){

if(m_cells[i][j].getIsAlive()){

total++;

}

}

}

}

}

cell.update_neighbors(total);

}



void apply_rules(Cell & cell){

// Les regles officielles du Jeu de la Vie

if(cell.getIsAlive()){

// Si elle a moins de 2 ou plus de 3 voisins, elle meurt

if(cell.getNeighbors() < 2 || cell.getNeighbors() > 3) {

cell.update_is_alive(false);

}

} else {

// Si elle est morte et a exactement 3 voisins, elle nait

if(cell.getNeighbors() == 3) {

cell.update_is_alive(true);

}

}

}



void display(){

for(int j = 0; j < m_height; ++j){

for(int i = 0; i < m_width; ++i){

if(m_cells[i][j].getIsAlive())

std::cout << alive;

else

std::cout << dead;

}

std::cout << "\n";

}

}



void set_alive(int x, int y){

if(x >= 0 && x < m_width && y >= 0 && y < m_height){

m_cells[x][y].update_is_alive(true);

}

}



void update(){

// Astuce : cacher le curseur du terminal

std::cout << "\x1B[?25l";



while(true){

// ÉTAPE 1 : On compte les voisins pour TOUTES les cellules d'abord

for(int j = 0; j < m_height; ++j){

for(int i = 0; i < m_width; ++i){

update_nb_neighbors(m_cells[i][j]);

}

}



// ÉTAPE 2 : On applique les règles pour TOUTES les cellules ensuite

for(int j = 0; j < m_height; ++j){

for(int i = 0; i < m_width; ++i){

apply_rules(m_cells[i][j]);

}

}



// ÉTAPE 3 : Affichage

// Cette ligne efface le terminal pour faire une vraie animation (ANSI escape codes)

std::cout << "\x1B[2J\x1B[H";

display();


std::this_thread::sleep_for(std::chrono::milliseconds(100));

}

}

};



int main(){

board game{40, 40};



// Configuration du Planeur (Glider)

game.set_alive(1, 0);

game.set_alive(2, 1);

game.set_alive(0, 2);

game.set_alive(1, 2);

game.set_alive(2, 2);



// Optionnel : Un "Clignotant" (Blinker) pour tester un objet statique

game.set_alive(10, 10);

game.set_alive(10, 11);

game.set_alive(10, 12);



game.update();

return 0;

}

