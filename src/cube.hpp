#pragma once
#include "../libs/glm/glm.hpp"
#include "../libs/glad.h"
#include "gl_types.hpp"
#include <vector>

class Cube
{
    bool m_is_white;
    GLfloat m_radius;
    GLuint m_VAO_id, m_VBO_id;

public:
    Cube(bool is_white, GLfloat radius)
        : m_is_white{is_white}, m_radius{radius}
    {
        GLfloat r = m_radius;

        GLfloat positions[] = {
            -r, -r, -r, r, -r, -r, -r, r, -r, r, r, -r,
            -r, -r, r, r, -r, r, -r, r, r, r, r, r};

        GLint indexes[] = {
            4, 5, 7, 4, 7, 6, 1, 0, 2, 1, 2, 3, 0, 4, 6, 0, 6, 2,
            5, 1, 3, 5, 3, 7, 6, 7, 3, 6, 3, 2, 0, 1, 5, 0, 5, 4};

        GLfloat colors_rgb[] = {0.0, 0.0, 0.0};
        GLfloat colors_white[] = {1.0, 1.0, 1.0};
        GLfloat *colors = (is_white) ? colors_white : colors_rgb;

        std::vector<GLfloat> vertices;
        for (int i = 0; i < 36; ++i)
        {
            for (int j = 0; j < 3; j++)
                vertices.push_back(positions[indexes[i] * 3 + j]);
            for (int j = 0; j < 3; j++)
                vertices.push_back(colors[j]);
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

    void draw(GLuint shaderProgram)
    {
        glBindVertexArray(m_VAO_id);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glEnableVertexAttribArray(VCOL_LOC);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f);

        glDisableVertexAttribArray(VCOL_LOC);

        glVertexAttrib3f(VCOL_LOC, 0.5f, 0.5f, 0.5f);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glBindVertexArray(0);
    }

    GLuint getVAO(){
        return m_VAO_id;
    }
};