#pragma once
#include "../libs/glad.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "cube.hpp"
#include "sim_params.hpp"
#include "render_params.hpp"

class App{

    SimParams sim_params;
    RenderParams render_params;


    //window
    GLFWwindow * window;
    int height = 1920;
    int width = 1080;


    std::vector<Cube*> rubiks;

    public:
        void initGL();
        void render();
        bool save_render(); // return true when over

        //STAITC !!! GLFW est écrit en C et attends un pointeur vers une fonction et App::framebuffer... a un paramètre caché this. en static pas de pointeur this   
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
        static void infos_GPU();
};