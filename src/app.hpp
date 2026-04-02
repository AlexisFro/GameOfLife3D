#pragma once
#include "../libs/glad.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "cube.hpp"
#include "sim_params.hpp"
#include "render_params.hpp"
#include <fstream>
#include <sstream>

class App{

    const char * m_app_name = "Automate Cellulaire 3D";

    SimParams sim_params;
    RenderParams render_params;


    GLuint ssboGridA, ssboGridB;
    bool useGridA;

    //window
    Cube* cube_blanc;
    Cube* cube_noir;

    GLFWwindow * window;
    int height = 1080;
    int width = 1920;
    bool m_render_mode = 0;

    //Shaders

    GLuint computeProgram; // Ton programme de calcul
    GLuint shaderProgram;  // Ton programme de dessin (Vertex + Fragment)
    


    std::vector<Cube*> rubiks;

    public:
        void initGL(int mode);
        void tearGL();
        void render();
        bool save_render(); // return true when over
        void initWindow();
        void compileShaders();
        void initbuffers();
        void run();
        std::string loadShaderSource(const std::string& filepath) ;
        GLuint compileSingleShader(GLenum type, const char* source, const std::string& name);
        ~App();

        //STAITC !!! GLFW est écrit en C et attends un pointeur vers une fonction et App::framebuffer... a un paramètre caché this. en static pas de pointeur this   
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
        static void infos_GPU();
};
