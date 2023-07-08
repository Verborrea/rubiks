#ifndef CUBO_H
#define CUBO_H

#include <iostream>

#include "include/matrix.h"
#include "include/shader.h"

#define WHITE  0
#define YELLOW 1
#define BLUE   2
#define GREEN  3
#define ORANGE 4
#define RED    5
#define BLACK  6

class Cubo
{
private:
    unsigned int VBO[7];
    unsigned int EBO[6];
    unsigned int VAO[6];

    void getCaraColor(float *cara, unsigned int *idx, int color);

    vec4 vertex[8];
    Shader *shader;
public:
    Cubo();
    ~Cubo();

    mat4 model;
    vec3 center;

    void draw();
    void setColors(int L, int R, int D, int U, int B, int F);
    void setShader(Shader* sh) { shader = sh; }

    void rotateVertex(float angle, vec3 vec);
    void translateVertex(vec3 vec);
};

Cubo::Cubo()
{
    #ifdef _WIN32
        gladLoadGL(glfwGetProcAddress);
    #endif

    float c = 0.0f;

    vertex[0] = vec4(c + 0.5f, c + 0.5f, c + 0.5f, 1.0f);
    vertex[1] = vec4(c - 0.5f, c + 0.5f, c + 0.5f, 1.0f);
    vertex[2] = vec4(c + 0.5f, c - 0.5f, c + 0.5f, 1.0f);
    vertex[3] = vec4(c - 0.5f, c - 0.5f, c + 0.5f, 1.0f);
    vertex[4] = vec4(c + 0.5f, c + 0.5f, c - 0.5f, 1.0f);
    vertex[5] = vec4(c - 0.5f, c + 0.5f, c - 0.5f, 1.0f);
    vertex[6] = vec4(c + 0.5f, c - 0.5f, c - 0.5f, 1.0f);
    vertex[7] = vec4(c - 0.5f, c - 0.5f, c - 0.5f, 1.0f);
    
    glGenVertexArrays(6, VAO);
    glGenBuffers(7, VBO);
    glGenBuffers(6, EBO);

    setColors(ORANGE, RED, WHITE, YELLOW, GREEN, BLUE);
    model = mat4(1.0f);
}

void Cubo::getCaraColor(float *cara, unsigned int *idx, int color)
{
    cara[idx[0]*2] = float(color + 1) * 0.125f; cara[idx[0]*2+1] = 1.0f;
    cara[idx[1]*2] = float(color)     * 0.125f; cara[idx[1]*2+1] = 1.0f;
    cara[idx[2]*2] = float(color + 1) * 0.125f; cara[idx[2]*2+1] = 0.0f;
    cara[idx[3]*2] = float(color)     * 0.125f; cara[idx[3]*2+1] = 0.0f;
};

Cubo::~Cubo()
{
    glDeleteVertexArrays(6, VAO);
    glDeleteBuffers(7, VBO);
    glDeleteBuffers(6, EBO);
}

void Cubo::draw() {
    for (int i = 0; i < 6; i++)
    {
        shader->setMat4("model", model);
        glBindVertexArray(VAO[i]);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);
    }    
}

void Cubo::setColors(int L, int R, int D, int U, int B, int F) {

    float cube_vertex[8][3];

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++) 
            cube_vertex[i][j] = vertex[i][j];

    unsigned int caras[6][4] = {
        {1,5,3,7},  // L
        {4,0,6,2},  // R
        {2,3,6,7},  // D
        {4,5,0,1},  // U
        {5,4,7,6},  // B
        {0,1,2,3},  // F
    };

    float colores_caras[6][16];
    
    getCaraColor(colores_caras[0], caras[0], L);
    getCaraColor(colores_caras[1], caras[1], R);
    getCaraColor(colores_caras[2], caras[2], D);
    getCaraColor(colores_caras[3], caras[3], U);
    getCaraColor(colores_caras[4], caras[4], B);
    getCaraColor(colores_caras[5], caras[5], F);

    // openGL buffers stuff ===================================================

    for (int i = 0; i < 6; i++) {
        glBindVertexArray(VAO[i]);
        // position attribute
        glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertex), cube_vertex, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        // texture coord attribute
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colores_caras[i]), colores_caras[i], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);

        // ebo de indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(caras[i]), caras[i], GL_STATIC_DRAW); 
    }
}

void Cubo::rotateVertex(float angle, vec3 vec)
{
    mat4 rot = rotate(mat4(1.0f), angle, vec);
    for (int i = 0; i < 8; i++)
        vertex[i] = rot * vertex[i];

    float cube_vertex[8][3];

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++) 
            cube_vertex[i][j] = vertex[i][j];
        
    for (int i = 0; i < 6; i++) {
        glBindVertexArray(VAO[i]);
        // position attribute
        glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertex), cube_vertex, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);
    }
}

void Cubo::translateVertex(vec3 vec)
{
    mat4 rot = translate(mat4(1.0f), vec);
    for (int i = 0; i < 8; i++)
        vertex[i] = rot * vertex[i];

    float cube_vertex[8][3];

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++) 
            cube_vertex[i][j] = vertex[i][j];
        
    for (int i = 0; i < 6; i++) {
        glBindVertexArray(VAO[i]);
        // position attribute
        glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertex), cube_vertex, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);
    }
    center += vec;
}

#endif  // CUBO_H