#include "app.hpp"
#include "app.hpp"
#include "cube.hpp" // Inclure ta classe Cube
#include <iostream>
#include <ctime>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../libs/stb_image_write.h" // N'oublie pas de télécharger ce fichier et de le mettre dans ton dossier !

#ifdef _WIN32
    #include <direct.h>   // Requis pour _mkdir sous Windows
#else
    #include <sys/stat.h> // Requis pour mkdir sous Mac/Linux
#endif

// Inclusions de GLM pour la caméra et les rotations
#include "../libs/glm/glm.hpp"
#include "../libs/glm/gtc/matrix_transform.hpp"
#include "../libs/glm/gtc/type_ptr.hpp"

void App::initGL(int mode) {
    std::cout << "--- DEMARRAGE DE L'APPLICATION ---" << std::endl;
    
    // 1. Définir le mode (Temps réel ou Rendu vidéo)
    m_render_mode = (mode == 2);

    // 2. Lancer les étapes d'initialisation une par une
    initWindow();
    infos_GPU();
    compileShaders();
    initbuffers();
    
    // 3. Démarrer la boucle principale
    run();
}

// ... ta fonction framebuffer_size_callback reste identique ...

void App::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
} 

bool App::save_render(){
    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    stbi_flip_vertically_on_write(true);

    char filename[128];
    sprintf(filename, "render/frame_%04d.bmp", render_params.frameCounter);
    stbi_write_bmp(filename, width, height, 3, pixels.data());

    render_params.frameCounter++;
    
    int pourcentage = (render_params.frameCounter * 100) / render_params.MAX_FRAMES;
    std::cout << "Rendu en cours : " << pourcentage << "% (" << render_params.frameCounter << "/" << render_params.MAX_FRAMES << " images)\r" << std::flush;

    if (render_params.frameCounter >= render_params.MAX_FRAMES) {
        std::cout << "\nTermine ! Vous pouvez compiler la video." << std::endl;
        return true; // Casse la boucle while
    }
    return false;
}
void App::infos_GPU(){
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* version = glGetString(GL_VERSION);

    if (!renderer || !vendor || !version) {
        std::cout << "ERREUR : Impossible de recuperer les infos GPU." << std::endl;
        return;
    }

    std::cout << "\n=== INFO MATERIEL ===" << std::endl;
    std::cout << "GPU     : " << renderer << std::endl;
    std::cout << "Marque  : " << vendor << std::endl;
    std::cout << "OpenGL  : " << version << std::endl;
    
    int max_invocations;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_invocations);
    
    int max_size[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &max_size[0]); // X
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &max_size[1]); // Y
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &max_size[2]); // Z

    std::cout << "\n--- LIMITES COMPUTE SHADER ---" << std::endl;
    std::cout << "Max Invocations par groupe (X*Y*Z) : " << max_invocations << std::endl;
    std::cout << "Max Taille X : " << max_size[0] << std::endl;
    std::cout << "Max Taille Y : " << max_size[1] << std::endl;
    std::cout << "Max Taille Z : " << max_size[2] << std::endl;
    std::cout << "=====================\n" << std::endl;
}
void App::tearGL(){
    glfwTerminate();
}
App::~App(){

    delete cube_blanc;
    delete cube_noir;

    // 2. On libère la mémoire de la carte graphique (VRAM)
    glDeleteBuffers(1, &ssboGridA);
    glDeleteBuffers(1, &ssboGridB);
    glDeleteProgram(computeProgram);
    glDeleteProgram(shaderProgram);

    tearGL();
}

void App::initWindow() {
    std::cout << "--- INITIALISATION FENETRE ---" << std::endl;

    // 1. Initialisation de la librairie GLFW
    if (!glfwInit()) {
        std::cerr << "Erreur: Echec de l'initialisation de GLFW." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 2. Configuration d'OpenGL (Version 4.3 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Anti-aliasing x4

    // 3. Gestion du mode invisible pour le rendu vidéo
    if (m_render_mode) {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); 
        std::cout << "Mode Rendu Video : Fenetre masquee." << std::endl;
    }

    // 4. Création de la fenêtre (On utilise l'attribut de la classe !)
    window = glfwCreateWindow(width, height, m_app_name, NULL, NULL);
    if (!window) { 
        std::cerr << "Erreur: Creation de la fenetre echouee." << std::endl;
        glfwTerminate(); 
        exit(EXIT_FAILURE); 
    }
    glfwMakeContextCurrent(window);

    // 5. Chargement des pointeurs de fonctions OpenGL avec GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur: Initialisation de GLAD echouee." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 6. Configuration des options graphiques globales
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); // Ignore les faces cachées
    glCullFace(GL_BACK);    // Ne dessine que l'extérieur
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f); // Couleur de fond (Bleu nuit)
}

GLuint App::compileSingleShader(GLenum type, const char* source, const std::string& name) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Vérification des erreurs NVIDIA / AMD
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERREUR COMPILATION [" << name << "] :\n" << infoLog << std::endl;
    }
    return shader;
}

std::string App::loadShaderSource(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERREUR FATALE : Impossible d'ouvrir le fichier shader : " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ------------------------------------------------------------------
// FONCTION PRINCIPALE : Assemble tous les programmes GPU
// ------------------------------------------------------------------
void App::compileShaders() {
    std::cout << "--- COMPILATION DES SHADERS ---" << std::endl;

    // 1. On lit le texte depuis tes vrais fichiers .glsl (que tu auras créés dans un dossier "shaders")
    std::string compCode = loadShaderSource("shaders/compute_shader.comp");
    std::string vertCode = loadShaderSource("shaders/vertex_shader.vert");
    std::string fragCode = loadShaderSource("shaders/fragment_shader.frag");

    // 2. On compile le Compute Program
    GLuint computeShader = compileSingleShader(GL_COMPUTE_SHADER, compCode.c_str(), "COMPUTE SHADER");
    computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);
    glDeleteShader(computeShader);

    // 3. On compile le Render Program
    GLuint vertexShader = compileSingleShader(GL_VERTEX_SHADER, vertCode.c_str(), "VERTEX SHADER");
    GLuint fragmentShader = compileSingleShader(GL_FRAGMENT_SHADER, fragCode.c_str(), "FRAGMENT SHADER");
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    std::cout << "Shaders compiles avec succes !" << std::endl;
}

void App::initbuffers(){
    std::cout << "--- INITIALISATION DES BUFFERS ---" << std::endl;

    int totalCells = sim_params.sizeGrid * sim_params.sizeGrid * sim_params.sizeGrid;
    std::vector<int> initialGrid(totalCells, 0);

    // Initialisation aléatoire : 20% de densité
    srand(time(NULL));
    for (int i = 0; i < totalCells; i++) {
        if ((rand() % 100) < 7) initialGrid[i] = 1;
    }

    useGridA = true; // Assure-toi que c'est un attribut bool dans app.hpp !

    glGenBuffers(1, &ssboGridA);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboGridA);
    glBufferData(GL_SHADER_STORAGE_BUFFER, totalCells * sizeof(int), initialGrid.data(), GL_DYNAMIC_COPY);

    glGenBuffers(1, &ssboGridB);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboGridB);
    glBufferData(GL_SHADER_STORAGE_BUFFER, totalCells * sizeof(int), NULL, GL_DYNAMIC_COPY);
    
    // Au lieu de créer les cubes en variables locales, on les stocke dans la classe
    // Si tu as déclaré "Cube* cube_blanc;" dans app.hpp :
    cube_blanc = new Cube(true, 1.0f);
    cube_noir = new Cube(false, 1.0f);
}

void App::run() {
    float lastStepTime = 0.0f;
    float virtualTime = 0.0f; 
    int totalCells = sim_params.sizeGrid * sim_params.sizeGrid * sim_params.sizeGrid;

    // On s'assure que le dossier existe
    #ifdef _WIN32
        _mkdir("render");
    #else
        mkdir("render", 0777);
    #endif

    while(!glfwWindowShouldClose(window)) {
        
        float currentTime;
        if (m_render_mode) {
            currentTime = virtualTime; 
            virtualTime += (1.0f / render_params.FPS); 
        } else {
            currentTime = (float)glfwGetTime();
        }

        // --- COMPUTE SHADER ---
        if (currentTime > lastStepTime + sim_params.intervalleSimulation) {
            glUseProgram(computeProgram);
            glUniform1i(glGetUniformLocation(computeProgram, "uSize"), sim_params.sizeGrid);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, useGridA ? ssboGridA : ssboGridB);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, useGridA ? ssboGridB : ssboGridA);

            int numGroups = (sim_params.sizeGrid + 7) / 8;
            glDispatchCompute(numGroups, numGroups, numGroups);

            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

            useGridA = !useGridA;
            lastStepTime = currentTime;
        }

        // --- RENDU ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        if (!m_render_mode) {
            glfwGetFramebufferSize(window, &width, &height);
        }
        
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)width/height, 0.1f, 2000.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -sim_params.sizeGrid * sim_params.espacement * 0.5f));
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), currentTime * 0.2f, glm::vec3(0, 1, 0));
        rotation = glm::rotate(rotation, currentTime * 0.05f, glm::vec3(1, 0, 0));
        glm::mat4 vp = projection * view * rotation; 

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "VP"), 1, GL_FALSE, glm::value_ptr(vp));
        glUniform1i(glGetUniformLocation(shaderProgram, "uGridSize"), sim_params.sizeGrid);
        glUniform1f(glGetUniformLocation(shaderProgram, "uSpacing"), sim_params.espacement);
        glUniform1f(glGetUniformLocation(shaderProgram, "uTime"), currentTime);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, useGridA ? ssboGridA : ssboGridB);

        glBindVertexArray(cube_blanc->getVAO()); 
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, totalCells); 
        glBindVertexArray(0);
        
        if (m_render_mode) {
            if(save_render()) break;
        } 

        glfwSwapBuffers(window);
        
        if (!m_render_mode) {
            glfwPollEvents();
        }
    }
}