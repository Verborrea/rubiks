#ifndef RUBIK_H
#define RUBIK_H

#include <string>
#include <vector>
#include <map>
#include <queue>

#include "cubo.hpp"

#define FRONT 0
#define BACK  1
#define UP    2
#define DOWN  3
#define LEFT  4
#define RIGHT 5

#define F_PRIME 6
#define B_PRIME 7
#define U_PRIME 8
#define D_PRIME 9
#define L_PRIME 10
#define R_PRIME 11

class Rubik
{
private:
    unsigned int camadas[6][9] = {
        { 4, 0, 1, 2, 5, 8, 7, 6, 3},   // FRONT
        {22,20,19,18,21,24,25,26,23},   // BACK
        {10,18,19,20,11, 2, 1, 0, 9},   // UP
        {16, 6, 7, 8,17,26,25,24,15},   // DOWN
        {12,18, 9, 0, 3, 6,15,24,21},   // LEFT
        {14, 2,11,20,23,26,17, 8, 5}    // RIGHT
    };

    float animation_counter;
    const float animation_incr = M_PI_2 / 10.0f;
    int   current_face;
    vec3  face_vector;
    bool  animating;
    bool  needUpdate;

    std::queue<char> movements_queue;

    void setCurrentMovement(char move);
    void updateCamada(char move);
    void updateCaraIndex(int face, int *faces, int *index);
public:
    Rubik(Shader *sh);
    ~Rubik();

    void move(std::string movements);
    void draw();
    void update();

    std::string moves;
    std::vector<Cubo *> cubos;
    std::vector<vec3> rotations;
    Shader *shader;
    bool active;    // indica si se muestra en pantalla o no
};

Rubik::Rubik(Shader *sh) : shader(sh)
{
    animation_counter = 0.0f;
    current_face      = FRONT;
    face_vector       = vec3(0.0f, 0.0f, 1.0f);
    animating         = false;
    needUpdate        = false;
    active = false;

    rotations = {
        vec3( 1.0f, 0.0f, 0.0f),
        vec3( 0.0f, 0.0f, 0.0f),
        vec3( 1.0f, 0.0f, 0.0f),
        vec3( 0.0f, 1.0f, 0.0f),
        vec3( 0.0f, 0.0f, 0.0f),
        vec3( 0.0f,-1.0f, 0.0f),
        vec3(-1.0f, 0.0f, 0.0f),
        vec3( 0.0f, 0.0f, 0.0f),
        vec3(-1.0f, 0.0f, 0.0f),

        vec3( 0.0f, 1.0f, 0.0f),
        vec3( 1.0f, 0.0f, 0.0f),
        vec3( 0.0f,-1.0f, 0.0f),
        vec3( 0.0f, 1.0f, 0.0f),
        vec3( 0.0f, 0.0f, 0.0f),
        vec3( 0.0f,-1.0f, 0.0f),
        vec3( 0.0f, 1.0f, 0.0f),
        vec3(-1.0f, 0.0f, 0.0f),
        vec3( 0.0f,-1.0f, 0.0f),

        vec3( 1.0f, 0.0f, 0.0f),
        vec3( 1.0f, 0.0f, 0.0f),    // 180deg
        vec3( 1.0f, 0.0f, 0.0f),
        vec3( 0.0f, 1.0f, 0.0f),
        vec3( 1.0f, 0.0f, 0.0f),    // 180deg
        vec3( 0.0f,-1.0f, 0.0f),
        vec3(-1.0f, 0.0f, 0.0f),
        vec3( 1.0f, 0.0f, 0.0f),    // 180deg
        vec3(-1.0f, 0.0f, 0.0f),
    };

    for (int i = 0; i < 27; i++) {
        cubos.emplace_back(new Cubo);
        cubos[i]->setShader(shader);
    }

    cubos[0]->setColors(ORANGE, BLACK, BLACK, YELLOW, BLACK, BLUE);
    cubos[1]->setColors(BLACK, BLACK, BLACK, YELLOW, BLACK, BLUE);
    cubos[2]->setColors(BLACK, RED, BLACK, YELLOW, BLACK, BLUE);
    cubos[3]->setColors(ORANGE, BLACK, BLACK, BLACK, BLACK, BLUE);
    cubos[4]->setColors(BLACK, BLACK, BLACK, BLACK, BLACK, BLUE);
    cubos[5]->setColors(BLACK, RED, BLACK, BLACK, BLACK, BLUE);
    cubos[6]->setColors(ORANGE, BLACK, WHITE, BLACK, BLACK, BLUE);
    cubos[7]->setColors(BLACK, BLACK, WHITE, BLACK, BLACK, BLUE);
    cubos[8]->setColors(BLACK, RED, WHITE, BLACK, BLACK, BLUE);

    cubos[9]->setColors(ORANGE, BLACK, BLACK, YELLOW, BLACK, BLACK);
    cubos[10]->setColors(BLACK, BLACK, BLACK, YELLOW, BLACK, BLACK);
    cubos[11]->setColors(BLACK, RED, BLACK, YELLOW, BLACK, BLACK);
    cubos[12]->setColors(ORANGE, BLACK, BLACK, BLACK, BLACK, BLACK);
    cubos[13]->setColors(BLACK, BLACK, BLACK, BLACK, BLACK, BLACK);
    cubos[14]->setColors(BLACK, RED, BLACK, BLACK, BLACK, BLACK);
    cubos[15]->setColors(ORANGE, BLACK, WHITE, BLACK, BLACK, BLACK);
    cubos[16]->setColors(BLACK, BLACK, WHITE, BLACK, BLACK, BLACK);
    cubos[17]->setColors(BLACK, RED, WHITE, BLACK, BLACK, BLACK);

    cubos[18]->setColors(ORANGE, BLACK, BLACK, YELLOW, GREEN, BLACK);
    cubos[19]->setColors(BLACK, BLACK, BLACK, YELLOW, GREEN, BLACK);
    cubos[20]->setColors(BLACK, RED, BLACK, YELLOW, GREEN, BLACK);
    cubos[21]->setColors(ORANGE, BLACK, BLACK, BLACK, GREEN, BLACK);
    cubos[22]->setColors(BLACK, BLACK, BLACK, BLACK, GREEN, BLACK);
    cubos[23]->setColors(BLACK, RED, BLACK, BLACK, GREEN, BLACK);
    cubos[24]->setColors(ORANGE, BLACK, WHITE, BLACK, GREEN, BLACK);
    cubos[25]->setColors(BLACK, BLACK, WHITE, BLACK, GREEN, BLACK);
    cubos[26]->setColors(BLACK, RED, WHITE, BLACK, GREEN, BLACK);
}

Rubik::~Rubik()
{
    for (int i = 0; i < 27; i++) {
        delete cubos[i];
    }
}

void Rubik::update()
{   
    if (animating) {
        needUpdate = true;
        if (animation_counter < M_PI_2) {

            // animar las camadas segun el giro
            for (int i = 0; i < 9; i++)
            {
                int idx = camadas[current_face][i];
                cubos[idx]->rotateVertex(animation_incr, face_vector);
            }

            animation_counter += animation_incr;
        } else {
            animating = false;
        }
    } else {
        // actualizar las camadas segun el giro
        if (needUpdate) {
            updateCamada(movements_queue.front());
            movements_queue.pop();
            needUpdate = false;
        }

        if (!movements_queue.empty()) {
            animation_counter = 0.0f;
            setCurrentMovement(movements_queue.front());
            animating = true;
        }
    }
}

void Rubik::draw()
{
    for (int i = 0; i < 27; i++)
    {
        cubos[i]->draw();
    }
}

void Rubik::move(std::string input)
{
    std::map<char, char> mov_map{
        {'F', FRONT},
        {'L', LEFT},
        {'R', RIGHT},
        {'U', UP},
        {'D', DOWN},
        {'B', BACK},
    };

    int c = 0;
    while (c < input.size()) {
        while (input[c] == ' ') c++;
        char curr = mov_map[input[c]];

        moves += input[c];

        if (input[c + 1] == '\'') {
            curr += 6;
            c++;

            moves += '\'';
        }
        movements_queue.push(curr);
        
        if (input[c + 1] == '2') {
            movements_queue.push(curr);
            c++;

            moves += '2';
        }
        c++;
        moves += ' ';
    }
}

void Rubik::setCurrentMovement(char move)
{
    switch (move)
    {
        case FRONT:
            current_face = FRONT;
            face_vector = vec3( 0.0f, 0.0f,-1.0f);
        break;
        case F_PRIME:
            current_face = FRONT;
            face_vector = vec3( 0.0f, 0.0f, 1.0f);
        break;
        case BACK:
            current_face = BACK;
            face_vector = vec3( 0.0f, 0.0f, 1.0f);
        break;
        case B_PRIME:
            current_face = BACK;
            face_vector = vec3( 0.0f, 0.0f,-1.0f);
        break;
        case UP:
            current_face = UP;
            face_vector = vec3( 0.0f,-1.0f, 0.0f);
        break;
        case U_PRIME:
            current_face = UP;
            face_vector = vec3( 0.0f, 1.0f, 0.0f);
        break;
        case DOWN:
            current_face = DOWN;
            face_vector = vec3( 0.0f, 1.0f, 0.0f);
        break;
        case D_PRIME:
            current_face = DOWN;
            face_vector = vec3( 0.0f,-1.0f, 0.0f);
        break;
        case LEFT:
            current_face = LEFT;
            face_vector = vec3( 1.0f, 0.0f, 0.0f);
        break;
        case L_PRIME:
            current_face = LEFT;
            face_vector = vec3(-1.0f, 0.0f, 0.0f);
        break;
        case RIGHT:
            current_face = RIGHT;
            face_vector = vec3(-1.0f, 0.0f, 0.0f);
        break;
        case R_PRIME:
            current_face = RIGHT;
            face_vector = vec3( 1.0f, 0.0f, 0.0f);
        break;
    }
}

void replace(unsigned int *a, int *n)
{
    unsigned int tmp[9];
    for (int i = 0; i < 9; i++) {
        tmp[i] = a[n[i]];
    }
    for (int i = 0; i < 9; i++) {
        a[i] = tmp[i];
    }
}

void Rubik::updateCaraIndex(int face, int *faces, int *index)
{
    camadas[faces[0]][index[0]] = camadas[face][3];
    camadas[faces[0]][index[1]] = camadas[face][2];
    camadas[faces[0]][index[2]] = camadas[face][1];

    camadas[faces[1]][index[3]] = camadas[face][5];
    camadas[faces[1]][index[4]] = camadas[face][4];
    camadas[faces[1]][index[5]] = camadas[face][3];

    camadas[faces[2]][index[6]] = camadas[face][7];
    camadas[faces[2]][index[7]] = camadas[face][6];
    camadas[faces[2]][index[8]] = camadas[face][5];

    camadas[faces[3]][index[9]]  = camadas[face][1];
    camadas[faces[3]][index[10]] = camadas[face][8];
    camadas[faces[3]][index[11]] = camadas[face][7];
}

void Rubik::updateCamada(char move)
{
    int new_index[9] =   {0,7,8,1,2,3,4,5,6};
    int prime_index[9] = {0,3,4,5,6,7,8,1,2};

    switch (move)
    {
    case FRONT: {
        int faces[] = {RIGHT, DOWN,  LEFT,  UP};
        int index[] = {7,8,1, 1,2,3, 3,4,5, 5,6,7};
        updateCaraIndex(FRONT, faces, index); 
        replace(camadas[FRONT], new_index);
        break; }
    case F_PRIME: {
        int faces[] = {LEFT,  UP,    RIGHT, DOWN};
        int index[] = {3,4,5, 5,6,7, 7,8,1, 1,2,3};
        updateCaraIndex(FRONT, faces, index); 
        replace(camadas[FRONT], prime_index);
        break; }
    case BACK: {
        int faces[] = {LEFT, DOWN,   RIGHT, UP};
        int index[] = {7,8,1, 5,6,7, 3,4,5, 1,2,3};
        updateCaraIndex(BACK, faces, index);
        replace(camadas[BACK], new_index);
        break; }
    case B_PRIME: {
        int faces[] = {RIGHT, UP,    LEFT,  DOWN};
        int index[] = {3,4,5, 1,2,3, 7,8,1, 5,6,7};
        updateCaraIndex(BACK, faces, index);
        replace(camadas[BACK], prime_index); 
        break; }
    case UP: {
        int faces[] = {RIGHT, FRONT, LEFT,  BACK};
        int index[] = {1,2,3, 1,2,3, 1,2,3, 1,2,3};
        updateCaraIndex(UP, faces, index); 
        replace(camadas[UP], new_index);
        break; }
    case U_PRIME: {
        int faces[] = {LEFT,  BACK,  RIGHT, FRONT};
        int index[] = {1,2,3, 1,2,3, 1,2,3, 1,2,3};
        updateCaraIndex(UP, faces, index); 
        replace(camadas[UP], prime_index);
        break; }
    case DOWN: {
        int faces[] = {RIGHT, BACK,  LEFT,  FRONT};
        int index[] = {5,6,7, 5,6,7, 5,6,7, 5,6,7};
        updateCaraIndex(DOWN, faces, index);
        replace(camadas[DOWN], new_index); 
        break; }
    case D_PRIME: {
        int faces[] = {LEFT,  FRONT, RIGHT, BACK};
        int index[] = {5,6,7, 5,6,7, 5,6,7, 5,6,7};
        updateCaraIndex(DOWN, faces, index); 
        replace(camadas[DOWN], prime_index);
        break; }
    case LEFT: {
        int faces[] = {FRONT, DOWN,  BACK,  UP};
        int index[] = {7,8,1, 7,8,1, 3,4,5, 7,8,1};
        updateCaraIndex(LEFT, faces, index); 
        replace(camadas[LEFT], new_index);
        break; }
    case L_PRIME: {
        int faces[] = {BACK,  UP,    FRONT, DOWN};
        int index[] = {3,4,5, 7,8,1, 7,8,1, 7,8,1};
        updateCaraIndex(LEFT, faces, index); 
        replace(camadas[LEFT], prime_index);
        break; }
    case RIGHT: {
        int faces[] = {BACK,  DOWN,  FRONT, UP};
        int index[] = {7,8,1, 3,4,5, 3,4,5, 3,4,5};
        updateCaraIndex(RIGHT, faces, index); 
        replace(camadas[RIGHT], new_index);
        break; }
    case R_PRIME: {
        int faces[] = {FRONT, UP,    BACK,  DOWN};
        int index[] = {3,4,5, 3,4,5, 7,8,1, 3,4,5};
        updateCaraIndex(RIGHT, faces, index);
        replace(camadas[RIGHT], prime_index); 
        break; }
    }
}

#endif