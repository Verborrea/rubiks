#ifndef SNAKE_H
#define SNAKE_H

#include <list>
#include <random>
#include <queue>

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
    std::list<Cubo*> cubos;
    std::queue<vec3> movements;
    Shader *shader;
    vec3 last_dir;
    vec3 curr_dir;

    float timer = 0.0f;
    const float interval = 0.25f;    // cada cuantos seg se mueve la serpiente

    // Para la fruta
    Cubo *food;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<int> dist;
public:
    Snake(Shader *sh);
    ~Snake();

    bool alive;

    void grow();
    void update(Camera *camera, float deltaTime);
    void draw();
    void move(snake_direction dir);
};

Snake::Snake(Shader *sh) : shader(sh), gen(rd()), dist(-3, 3)
{
    alive = true;

    Cubo *head = new Cubo;
    head->setShader(shader);
    head->setColors(ORANGE, BLACK, BLACK, YELLOW, BLACK, BLUE);
    
    cubos.emplace_back(head);

    // añadir la primera comida al tablero
    srand(2352464);
    food = new Cubo;
    food->setShader(shader);
    food->setColors(rand()%7, rand()%7, rand()%7, rand()%7, rand()%7, rand()%7);
    food->translateVertex( vec3(2.0f, 0.0f, 0.0f) );
}

Snake::~Snake()
{
    for (auto cubo = cubos.begin(); cubo != cubos.end(); ++cubo) {
        delete *cubo;
    }
    delete food;
}

void Snake::grow()
{
    // reemplazar la cabeza por la comida
    cubos.emplace_front(food);

    // crear nueva comida en un lugar vacio
    food = new Cubo;
    food->setShader(shader);
    food->setColors(rand()%7, rand()%7, rand()%7, rand()%7, rand()%7, rand()%7);

    vec3 food_pos;
    bool choque = false;
    do
    {
        food_pos = vec3(dist(gen), dist(gen), 0.0f);
        choque = false;
        for (auto it = cubos.begin(); it != cubos.end(); ++it) {
            if (food_pos == (*it)->center) {
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

    if (timer >= interval) {
        timer = 0.0f;

        // cambiar la dirección si la pila de movs. no está vacía
        if(!movements.empty()) {
            curr_dir = movements.front();
            movements.pop();
        }

        // Si comió un cubo:
        if (cubos.front()->center + curr_dir == food->center) {
            grow();
        }

        // actualizar la posición del cuerpo
        for (auto it = std::prev(cubos.end()); it != cubos.begin(); --it) {
            vec3 dir_anterior = (*std::prev(it))->center - (*it)->center;
            (*it)->translateVertex(dir_anterior);
        }

        // actualizar la posición de la cabeza
        cubos.front()->translateVertex(curr_dir);

        // actualizar la posición de la cámara
        camera->Position[0] = cubos.front()->center[0];
        camera->Position[1] = cubos.front()->center[1];

        // Verificar que el jugador no haya chocado consigo mismo
        for (auto it = std::next(cubos.begin()); it != cubos.end(); ++it) {
            if (cubos.front()->center == (*it)->center) {
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

#endif