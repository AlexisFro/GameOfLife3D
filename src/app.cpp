#include "app.hpp"
#include "app.hpp"
#include "cube.hpp" // Inclure ta classe Cube
#include <iostream>
#include "automate_cellulaire.cpp"
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

const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 vPos;\n"
    "layout (location = 2) in vec3 iOffset;\n" // Position de l'instance
    "out vec3 WorldPos;\n"
    "out vec3 InstancePos;\n"
    "uniform mat4 VP;\n"
    "void main()\n"
    "{\n"
    "   vec3 pos = vPos + iOffset;\n"
    "   WorldPos = pos;\n"
    "   InstancePos = iOffset;\n" // Position centrale du cube pour le dégradé
    "   gl_Position = VP * vec4(pos, 1.0);\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "in vec3 WorldPos;\n"
    "in vec3 InstancePos;\n"
    "out vec4 FragColor;\n"
    "uniform float uTime;\n"
    "uniform float uGridSize;\n"
    "void main()\n"
    "{\n"
    "   // 1. FLAT SHADING : Calcul des normales à la volée pour définir les faces\n"
    "   vec3 dFdxPos = dFdx(WorldPos);\n"
    "   vec3 dFdyPos = dFdy(WorldPos);\n"
    "   vec3 normal = normalize(cross(dFdxPos, dFdyPos));\n"
    "   \n"
    "   // 2. PALETTE DE COULEURS : Dégradé Spatial vibrant (Cool -> Chaud)\n"
    "   // Utilise InstancePos normalisée. Y est le facteur principal cool-vers-chaud.\n"
    "   float yNorm = (InstancePos.y / uGridSize) * 0.5 + 0.5; // [-1, 1] -> [0, 1]\n"
    "   float xNorm = (InstancePos.x / uGridSize) * 0.5 + 0.5;\n"
    "   float zNorm = (InstancePos.z / uGridSize) * 0.5 + 0.5;\n"
    "   \n"
    "   // Couleurs clés pour le dégradé\n"
    "   vec3 colorCyan = vec3(0.0, 0.8, 1.0); // Bas / Cool\n"
    "   vec3 colorMagenta = vec3(1.0, 0.2, 0.7); // Milieu / Chaud\n"
    "   vec3 colorGold = vec3(1.0, 0.8, 0.1); // Haut / Chaud\n"
    "   vec3 colorDeepBlue = vec3(0.05, 0.05, 0.15);\n"
    "   \n"
    "   // Mixer les couleurs sur l'axe Y\n"
    "   vec3 baseColor = (yNorm < 0.5) ? mix(colorCyan, colorMagenta, yNorm * 2.0) : mix(colorMagenta, colorGold, (yNorm - 0.5) * 2.0);\n"
    "   // Ajouter de la variation subtile sur X/Z pour la richesse de la couleur\n"
    "   baseColor = mix(baseColor, vec3(1.0, 0.1, 0.3), abs(InstancePos.x) / uGridSize * 0.1);\n"
    "   \n"
    "   // 3. ÉCLAIRAGE DIRECTIONNEL (Soleil d'en haut-droite-avant)\n"
    "   vec3 lightDir = normalize(vec3(1.0, 1.5, 0.5));\n"
    "   float diffuse = max(dot(normal, lightDir), 0.0);\n"
    "   \n"
    "   // 4. ÉCLAIRAGE POINT CENTRAL/BLOOM (Le point brillant dans l'image)\n"
    "   // Positionné légèrement en dessous du centre pour l'effet de l'image.\n"
    "   vec3 pointLightPos = vec3(0.0, -10.0, 0.0);\n"
    "   float distToPoint = length(InstancePos - pointLightPos);\n"
    "   float pointLightIntensity = 1.0 / (1.0 + 0.01 * distToPoint * distToPoint); // Affaiblissement\n"
    "   pointLightIntensity = pow(pointLightIntensity, 2.0); // Concentrer l'effet\n"
    "   vec3 pointColor = vec3(1.0, 0.2, 0.7); // Couleur de la lumière point\n"
    "   \n"
    "   // 5. Matériau : Ajouter de l'émission basée sur la couleur de base\n"
    "   vec3 emission = baseColor * 0.4;\n"
    "   \n"
    "   // 6. Combinaison finale de la lumière\n"
    "   float lightAmb = 0.3; // Ambiante pour ne pas avoir de noir total\n"
    "   vec3 litColor = baseColor * (lightAmb + diffuse * 0.7) + (pointColor * pointLightIntensity * 15.0) + emission;\n"
    "   \n"
    "   // 7. FOG (Brouillard Profond Cosmique)\n"
    "   float dist = length(InstancePos);\n"
    "   float fogFactor = exp(-dist / (uGridSize * 1.8)); // Brouillard plus profond\n"
    "   vec3 cosmicBackground = vec3(0.005, 0.005, 0.01); // Bleu-Noir profond\n"
    "   \n"
    "   vec3 finalColor = mix(cosmicBackground, litColor, clamp(fogFactor, 0.0, 1.0));\n"
    "   \n"
    "   FragColor = vec4(finalColor, 1.0f);\n"
    "}\n\0";

void App::initGL(){
    std::cout << "--- DEMARRAGE DU PROGRAMME ---" << std::endl;
    std::cout << "1. Mode Temps Reel (Affichage fenetre, interactif)" << std::endl;
    std::cout << "2. Mode Rendu Video (Pas d'affichage, creation de 1200 images pour 20s)" << std::endl;
    std::cout << "Choisissez le mode (1 ou 2) : ";
    
    int modeChoice;
    std::cin >> modeChoice;
    bool isOfflineRender = (modeChoice == 2);

    std::cout << __func__ << std::endl;

    // 1. INITIALISATION GLFW / GLAD
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Active l'anti-aliasing x4

    // --- LA MAGIE EST ICI ---
    if (isOfflineRender) {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Cache la fenêtre !
        std::cout << "Lancement du rendu en arriere-plan... Veuillez patienter." << std::endl;
    }


    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Rubik's Cube Damier", NULL, NULL);
    if (!window) { glfwTerminate(); return; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return;
    
    App::infos_GPU();

    glEnable(GL_MULTISAMPLE);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 

    glEnable(GL_CULL_FACE); // Ignore les faces cachées
    glCullFace(GL_BACK);    // Ne dessine que l'extérieur

    // 2. COMPILATION SHADERS

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f); // Un bleu nuit très sombre

    // 2. PRÉPARATION DE L'AUTOMATE
    Board3D board(sim_params.sizeGrid);
    
    Cube cube_blanc(true, 0.30f);
    Cube cube_noir(false, 0.30f);

    // --- Dans App::initGL ---
    GLuint instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    // On réserve de la place pour sizeGrid^3 positions (au cas où tout est vivant)
    glBufferData(GL_ARRAY_BUFFER, sim_params.sizeGrid * sim_params.sizeGrid * sim_params.sizeGrid * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);

    // Configurer le VAO du cube pour accepter les instances
    glBindVertexArray(cube_blanc.getVAO()); // Ajoute un getter getVAO() dans ta classe Cube
    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    // CRUCIAL : Dit à OpenGL que cet attribut avance de 1 par INSTANCE et non par sommet
    glVertexAttribDivisor(2, 1); 
    glBindVertexArray(0);

    
    
    // Initialisation aléatoire : 20% de chances d'être vivant au départ
    srand(time(NULL));
    for (int x = 0; x < sim_params.sizeGrid; x++) {
        for (int y = 0; y < sim_params.sizeGrid; y++) {
            for (int z = 0; z < sim_params.sizeGrid; z++) {
                if ((rand() % 100) < 10) { // 20% de densité
                    board.set_alive(x, y, z);
                }
            }
        }
    }
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    // Récupération des locations des Uniforms
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
    GLint timeLoc = glGetUniformLocation(shaderProgram, "uTime");
    GLint gridSizeLoc = glGetUniformLocation(shaderProgram, "uGridSize");
    GLint aliveLoc = glGetUniformLocation(shaderProgram, "uIsAlive");

    std::vector<glm::vec3> instancePositions;
    bool needsUpdate = true; // Flag pour mettre à jour le GPU
    float lastStepTime = 0.0f;
     // Simulation rapide
    float centre = (sim_params.sizeGrid - 1) / 2.0f;

    float virtualTime = 0.0f; // Le faux temps pour la vidéo

    // On s'assure que le dossier existe
    #ifdef _WIN32
        _mkdir("render");
    #else
        mkdir("render", 0777);
    #endif


    while(!glfwWindowShouldClose(window)) {
        
        // gestion du temps 
        float currentTime;
        if (isOfflineRender) {
            currentTime = virtualTime; 
            virtualTime += (1.0f / render_params.FPS); // On avance de 1/60e de seconde exactement
        } else {
            currentTime = (float)glfwGetTime(); // Temps normal pour le direct
        }

        // logique de l'automate
        if (currentTime > lastStepTime + sim_params.intervalleSimulation) {
            board.step();
            lastStepTime = currentTime;
            needsUpdate = true;
        }

        float espacement = sim_params.espacement;
        // mise à jour du buffer gpu
        if (needsUpdate) {
            instancePositions.clear();
            for(int x = 0; x < sim_params.sizeGrid; ++x) {
                for(int y = 0; y < sim_params.sizeGrid; ++y) {
                    for (int z = 0; z < sim_params.sizeGrid; ++z) {
                        if(board.is_alive(x, y, z)) {
                            instancePositions.push_back(glm::vec3(
                                (x - centre) * espacement,
                                (y - centre) * espacement,
                                (z - centre) * espacement
                            ));
                        }
                    }
                }
            }
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, instancePositions.size() * sizeof(glm::vec3), instancePositions.data());
            needsUpdate = false;
        }

        // rendu
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        // En mode caché, glfwGetFramebufferSize peut renvoyer 0 sur certains OS, on force la taille 1080p
        if (!isOfflineRender) {
            glfwGetFramebufferSize(window, &width, &height);
        }
        
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)width/height, 0.1f, 800.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -sim_params.sizeGrid * 0.5));
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), currentTime * 0.2f, glm::vec3(0, 1, 0));
        rotation = glm::rotate(rotation, currentTime * 0.05f, glm::vec3(1, 0, 0));

        glUniform1f(glGetUniformLocation(shaderProgram, "uTime"), currentTime);
        glUniform1f(glGetUniformLocation(shaderProgram, "uGridSize"), (float)sim_params.sizeGrid);
        glm::mat4 vp = projection * view * rotation; // à mettre dans le vertex pour calculer dans le GPU
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "VP"), 1, GL_FALSE, glm::value_ptr(vp));

        if (!instancePositions.empty()) {
            glBindVertexArray(cube_blanc.getVAO());
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, (GLsizei)instancePositions.size());
            glBindVertexArray(0);
        }

        // --- ÉTAPE 4 : SAUVEGARDE (Si on est en mode rendu vidéo) ---
        if (isOfflineRender) {
            if(save_render()) break;
        } 

        // On swap les buffers (même si caché, c'est nécessaire pour OpenGL)
        glfwSwapBuffers(window);
        
        // On ne gère les événements (clavier/souris) qu'en temps réel
        if (!isOfflineRender) {
            glfwPollEvents();
        }
    }

    glfwTerminate();
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
    
    // Affichage de la progression dans la console
    int pourcentage = (render_params.frameCounter * 100) / render_params.MAX_FRAMES;
    std::cout << "Rendu en cours : " << pourcentage << "% (" << render_params.frameCounter << "/" << render_params.MAX_FRAMES << " images)\r" << std::flush;

    // Arrêt automatique quand on a atteint les 20 secondes
    if (render_params.frameCounter >= render_params.MAX_FRAMES) {
        std::cout << "\nTermine ! Vous pouvez compiler la video." << std::endl;
        return true; // Casse la boucle while
    }
    return false;
}
void App::infos_GPU(){
    const GLubyte* renderer = glGetString(GL_RENDERER); // Le nom du GPU
    const GLubyte* vendor = glGetString(GL_VENDOR);     // Le fabricant (NVIDIA, AMD, Intel...)
    const GLubyte* version = glGetString(GL_VERSION);   // La version d'OpenGL supportée

    std::cout << "\n=== INFO MATERIEL ===" << std::endl;
    std::cout << "GPU     : " << renderer << std::endl;
    std::cout << "Marque  : " << vendor << std::endl;
    std::cout << "OpenGL  : " << version << std::endl;
    std::cout << "=====================\n" << std::endl;
}