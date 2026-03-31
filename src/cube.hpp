#pragma once
#include "../libs/glm/glm.hpp"
#include "../libs/glad.h"
#include "gl_types.hpp"
#include <vector>

class Cube
{
    bool m_is_white; // color is white or black
    GLfloat m_radius;
    GLuint m_VAO_id, m_VBO_id;

public:
    Cube(bool is_white, GLfloat radius)
        : m_is_white{is_white}, m_radius{radius}
    {
        GLfloat r = m_radius;

        // Positions (Inchangé)
        GLfloat positions[] = {
            -r, -r, -r, r, -r, -r, -r, r, -r, r, r, -r,
            -r, -r, r, r, -r, r, -r, r, r, r, r, r};

        // Indices pour triangles (Inchangé)
        GLint indexes[] = {
            4, 5, 7, 4, 7, 6, 1, 0, 2, 1, 2, 3, 0, 4, 6, 0, 6, 2,
            5, 1, 3, 5, 3, 7, 6, 7, 3, 6, 3, 2, 0, 1, 5, 0, 5, 4};

        // Couleurs (Inchangé)
        GLfloat colors_rgb[] = {0.0, 0.0, 0.0};
        GLfloat colors_white[] = {1.0, 1.0, 1.0};
        GLfloat *colors = (is_white) ? colors_white : colors_rgb;

        // Création VBO entrelacé [Pos, Col] (Inchangé)
        std::vector<GLfloat> vertices;
        for (int i = 0; i < 36; ++i)
        {
            for (int j = 0; j < 3; j++)
                vertices.push_back(positions[indexes[i] * 3 + j]);
            for (int j = 0; j < 3; j++)
                vertices.push_back(colors[j]); // Utilise C0 directement
        }

        glGenVertexArrays(1, &m_VAO_id);
        glBindVertexArray(m_VAO_id);

        glGenBuffers(1, &m_VBO_id);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO_id);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        // VAA Pos (Inchangé)
        glVertexAttribPointer(VPOS_LOC, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(VPOS_LOC);

        // VAA Col (Inchangé)
        glVertexAttribPointer(VCOL_LOC, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(VCOL_LOC);

        glBindVertexArray(0);
    }

    ~Cube()
    {
        glDeleteBuffers(1, &m_VBO_id);
        glDeleteVertexArrays(1, &m_VAO_id);
    }
    Cube(const Cube &) = delete;
    Cube &operator=(const Cube &) = delete;

    // --- LA FONCTION DRAW MODIFIÉE ---
    // Elle prend l'ID du shader pour forcer la couleur grise des arêtes
    void draw(GLuint shaderProgram)
    {
        glBindVertexArray(m_VAO_id);

        // ===================================================================
        // PASSAGE 1 : DESSIN DES FACES PLEINES (Couleur du VBO : Noir ou Blanc)
        // ===================================================================
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // On s'assure que l'attribut de couleur du VBO est activé
        glEnableVertexAttribArray(VCOL_LOC);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ===================================================================
        // PASSAGE 2 : DESSIN DES ARÊTES (Mode Wireframe, Couleur Grise Forcée)
        // ===================================================================
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // 1. Pour éviter le Z-Fighting (clignotement lignes/faces)
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f); // "Rapproche" les lignes de la caméra

        // 2. FORCER LA COULEUR GRISE
        // On DÉSACTIVE l'attribut de couleur du VBO.
        // Du coup, le shader n'aura plus de donnée pour 'vCol'.
        glDisableVertexAttribArray(VCOL_LOC);

        // En OpenGL, si un attribut est désactivé, il utilise une valeur par défaut.
        // On fixe cette valeur par défaut à Gris Moyen (0.5, 0.5, 0.5).
        glVertexAttrib3f(VCOL_LOC, 0.5f, 0.5f, 0.5f);

        // On dessine (les mêmes triangles, mais en mode LIGNE)
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ===================================================================
        // NETTOYAGE DES ÉTATS
        // ===================================================================
        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Remet le mode plein par défaut
        
        // Note importante : On ne réactive pas VCOL_LOC ici, 
        // c'est le début du prochain draw() qui s'en chargera.

        glBindVertexArray(0);
    }

    GLuint getVAO(){
        return m_VAO_id;
    }
};