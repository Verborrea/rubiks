#ifndef SNAKE_H
#define SNAKE_H

#include <list>
#include <random>
#include <queue>

#include "include/camera.h"

#include "rubik.hpp"

enum snake_direction {
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT,
};

class Snake
{
private:
    std::list<Cubo*> cubos;
    std::queue<vec3> movements;
    Rubik  *rubik;
    vec3 last_dir;
    vec3 curr_dir;
    vec3 center_cola;

    float timer = 0.0f;
    float zoom_factor = 2.0f;
    const float interval = 0.20f;    // cada cuantos seg se mueve la serpiente

    // Para la fruta
    Cubo *food;
    int   food_idx;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<int> dist;
public:
    Snake(Rubik *rb);

    bool alive;

    void grow();
    void update(Camera *camera, float deltaTime);
    void draw();
    void move(snake_direction dir);
    void toRubik();
};

Snake::Snake(Rubik *rb) : rubik(rb), gen(rd()), dist(-3, 3)
{
    alive = true;

    cubos.emplace_back(rubik->cubos[0]);
    cubos.front()->rotateVertex(radians(90.0f), rubik->rotations[0]);
    cubos.front()->translateVertex( vec3(0.0f, 0.0f, 0.0f) );
    
    // añadir la primera comida al tablero
    food_idx = 1;
    food = rubik->cubos[food_idx];
    food->rotateVertex(radians(90.0f), rubik->rotations[1]);
    food->translateVertex( vec3(2.0f, 0.0f, 0.0f) );
}

void Snake::grow()
{
    // añadir la comida a la cola
    cubos.emplace_back(food);
    food->translateVertex(center_cola - food->center());

    // si se llegó a los 27 cubos -> to rubik
    if (cubos.size() == 27) {
        toRubik();
        return;
    }

    // crear nueva comida en un lugar vacio
    food = rubik->cubos[++food_idx];
    food->rotateVertex(radians(90.0f), rubik->rotations[food_idx]);
    if (food_idx == 19 || food_idx == 22 || food_idx == 25)
        food->rotateVertex(radians(90.0f), rubik->rotations[food_idx]);

    vec3 food_pos;
    bool choque = false;
    do
    {
        food_pos = vec3(dist(gen), dist(gen), 0.0f);
        choque = false;
        for (auto it = cubos.begin(); it != cubos.end(); ++it) {
            if (food_pos == (*it)->center()) {
                choque = true;
                break;
            }
        }
    } while (choque);
    food->translateVertex(food_pos);
}

void Snake::update(Camera *camera, float deltaTime)
{
    timer += deltaTime;

    // actualizar la posición de la cámara
    camera->Position[0] += curr_dir[0] * deltaTime * 5;
    camera->Position[1] += curr_dir[1] * deltaTime * 5;

    if (timer >= interval) {
        timer = 0.0f;

        // cambiar la dirección si la pila de movs. no está vacía
        if(!movements.empty()) {
            curr_dir = movements.front();
            movements.pop();
        }

        // guardar la posición de la cola para el grow
        center_cola = cubos.back()->center();

        // actualizar la posición del cuerpo
        for (auto it = std::prev(cubos.end()); it != cubos.begin(); --it) {
            vec3 dir_anterior = (*std::prev(it))->center() - (*it)->center();
            (*it)->translateVertex(dir_anterior);
        }

        // actualizar la posición de la cabeza
        cubos.front()->translateVertex(curr_dir);

        // Si comió un cubo:
        if (cubos.front()->center() == food->center()) {
            // retroceder la cámara
            camera->Position[2] += zoom_factor;
            zoom_factor = zoom_factor * 0.8;
            grow();
            if (cubos.size() == 27)  {
                camera->Position[2] = 10.0f;
                return;
            }
        }

        // Verificar que el jugador no haya chocado consigo mismo
        for (auto it = std::next(cubos.begin()); it != cubos.end(); ++it) {
            if (cubos.front()->center() == (*it)->center()) {
                alive = false;
                break;
            }
        }
    }
}

void Snake::draw()
{
    for (auto cubo = cubos.begin(); cubo != cubos.end(); ++cubo) {
        (*cubo)->draw();
    }
    food->draw();
}

void Snake::move(snake_direction dir)
{
    if (dir == SNAKE_UP && last_dir[1] == 0)
        movements.emplace( 0.0f, 1.0f, 0.0f);
    if (dir == SNAKE_DOWN && last_dir[1] == 0)
        movements.emplace( 0.0f,-1.0f, 0.0f);
    if (dir == SNAKE_RIGHT && last_dir[0] == 0)
        movements.emplace( 1.0f, 0.0f, 0.0f);
    if (dir == SNAKE_LEFT && last_dir[0] == 0)
        movements.emplace(-1.0f, 0.0f, 0.0f);

    last_dir = movements.back();
}

void Snake::toRubik()
{
    for (int i = 0; i < 27; i++) {
        rubik->cubos[i]->translateVertex(vec3() - rubik->cubos[i]->center());
        rubik->cubos[i]->rotateVertex(radians(-90.0f), rubik->rotations[i]);
        if (i == 19 || i == 22 || i == 25)
            rubik->cubos[i]->rotateVertex(radians(-90.0f), rubik->rotations[i]);
        rubik->cubos[i]->translateVertex(rubik->centros[i]);
    }

    rubik->active = true;
}

#endif