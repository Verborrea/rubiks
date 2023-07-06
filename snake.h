#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <random>

#include "include/camera.h"

#include "cubo.h"

enum snake_direction {
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT,
};

class Snake
{
private:
    std::vector<Cubo*> cubos;
    Shader *shader;

    vec3 last_pos;

    float timer = 0.0f;
    const float interval = 0.25f;    // cada cuantos seg se mueve la serpiente

    vec3 last_dir;

    // Para la posición de la fruta
    std::random_device rd;
    std::mt19937 generator;
    std::uniform_int_distribution<int> distribution;
public:
    Snake(Shader *sh);
    ~Snake();

    vec3 direccion;
    bool alive;

    void grow();
    void update(Camera *camera, float deltaTime, Cubo *food);
    void draw();
    void move(snake_direction dir);
};

Snake::Snake(Shader *sh) : shader(sh), generator(rd()), distribution(-3, 3)
{
    direccion = vec3(1.0f, 0.0f, 0.0f);
    alive = true; 

    Cubo *cubo = new Cubo;

    cubo->setShader(shader);
    cubo->setColors(ORANGE, BLACK, BLACK, YELLOW, BLACK, BLUE);
    cubo->translateVertex(last_pos);
    
    cubos.emplace_back(cubo);
}

Snake::~Snake()
{
    for (auto cubo = cubos.begin(); cubo != cubos.end(); ++cubo) {
        delete *cubo;
    }
}

void Snake::grow()
{
    Cubo *cubo = new Cubo;


    if (last_dir[0] == 0.0f && last_dir[1] == 0.0f) {
        last_pos = cubos.back()->center - direccion;
    } else {
        last_pos = cubos.back()->center - last_dir;
    }


    cubo->setShader(shader);
    cubo->setColors(rand()%7, rand()%7, rand()%7, rand()%7, rand()%7,rand()%7);
    cubo->translateVertex(last_pos);

    cubos.emplace_back(cubo);
}

void Snake::update(Camera *camera, float deltaTime, Cubo *food)
{
    timer += deltaTime;

    camera->Position += direccion * (deltaTime / interval);

    if (timer >= interval) {
        timer = 0.0f;

        // Actualiza la posición de cada cubo en función de su dirección correspondiente
        bool helper = true;
        for (int i = cubos.size() - 1; i > 0; --i) {
            if (helper) {
                helper = false;
                last_dir = cubos[i-1]->center - cubos[i]->center;
            }
            vec3 direccion_cubo_anterior = cubos[i-1]->center - cubos[i]->center;
            cubos[i]->translateVertex(direccion_cubo_anterior);
        }

        // Actualiza la posición del primer cubo (cabeza) según la dirección actual de la serpiente
        cubos[0]->translateVertex(direccion);

        // Si es la misma que de la comida -> moverla y crecer
        if (cubos[0]->center == food->center) {

            // TODO: hacer que se genere fuera del cuerpo de la serpiente
            food->translateVertex(vec3(
                distribution(generator),
                distribution(generator),
                0.0f));
            grow();
        }

        // Verificar que el jugador no haya chocado consigo mismo
        for (int i = 1; i < cubos.size(); i++) {
            if (cubos[0]->center == cubos[i]->center) {
                alive = false;
            }
        }
    }
}

void Snake::draw()
{
    for (auto cubo = cubos.begin(); cubo != cubos.end(); ++cubo) {
        (*cubo)->draw();
    }
}

void Snake::move(snake_direction dir)
{
    if (cubos.size() > 1) {
        if (dir == SNAKE_UP && (cubos[0]->center[0] != cubos[1]->center[0]))
            direccion = vec3( 0.0f, 1.0f, 0.0f);
        if (dir == SNAKE_DOWN && (cubos[0]->center[0] != cubos[1]->center[0]))
            direccion = vec3( 0.0f, -1.0f, 0.0f);
        if (dir == SNAKE_RIGHT && (cubos[0]->center[1] != cubos[1]->center[1]))
            direccion = vec3( 1.0f, 0.0f, 0.0f);
        if (dir == SNAKE_LEFT && (cubos[0]->center[1] != cubos[1]->center[1]))
            direccion = vec3(-1.0f, 0.0f, 0.0f);
    } else {
        if (dir == SNAKE_UP)
            direccion = vec3( 0.0f, 1.0f, 0.0f);
        if (dir == SNAKE_DOWN)
            direccion = vec3( 0.0f, -1.0f, 0.0f);
        if (dir == SNAKE_RIGHT)
            direccion = vec3( 1.0f, 0.0f, 0.0f);
        if (dir == SNAKE_LEFT)
            direccion = vec3(-1.0f, 0.0f, 0.0f);
    }
}

#endif