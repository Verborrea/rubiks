#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "include/camera.h"

#include "snake.hpp"
#include "rubik.hpp"

#include "Cube.hpp"
#include "Solver.hpp"
#include "Rubik2.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;
const char        *WIN_NAME   = "Rubik's Snake";

// objetos
Snake* snake;
Rubik* rubik;

// camera
Camera camera(vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;









Cube cube;

std::string translate(std::string path)
{
	string ret;

	for (int i = 0; path[i]; i++)
	{
		if (path[i] == '1')
			ret += " ";
		else if (path[i] == '3')
			ret += "\' ";
		else if (path[i] == '2')
			ret += "2 ";
		else
			ret += path[i];
	}
	return ret;
}

std::string mergeExtras(std::string moves){
	char prev = 0;
	int num = 0;
	string out = "";
	for (int i = 0; i < moves.size(); i+=2){
		if (moves[i] == prev){
			num = (num + moves[i + 1] - '0') % 4;
			out = out.substr(0,out.size() - 2);
		}
		else{
			prev = moves[i];
			num = moves[i + 1] - '0';
		}
		if (num != 0){
			out += moves[i];
			out += (num + '0');
		}
		else{
			prev = 0;
		}
	}
	return out;
}

void shuffle(std::string moves)
{
    for (int i = 0; moves[i]; i++)
        if (moves[i] == 'F' || moves[i] == 'R' || moves[i] == 'U' ||
            moves[i] == 'B' || moves[i] == 'L' || moves[i] == 'D')
        {
            int num = 1;
            if (moves[i + 1] == '\'')
                num = 3;
            else if (moves[i + 1] == '2')
                num = 2;
            cube.rotCube(moves[i], num);
        }
	cube.getColor();
	cout << GREEN << "Cube shuffle complete!\n";
}

void hashSolve(Cube *solverCube, Solver *s, std::string *output)
{
	char	face;
	int		num;
	int		i = 0;
	Cube 	c;
	for (int phase = 1; phase <= 4; phase++) {
		cout << YELLO << "Phase moves: " << endl;
		while (s->getPhaseId(*solverCube, phase) != s->phaseGoal[phase]) {
			string path = phaseHash[phase -1][s->getPhaseId(*solverCube, phase)];
			if (path == "") {
				cout << RED << "Solution not found" << endl;
				exit(1);				
			}
			cout << WHITE << '\t' << translate(path) << endl;
			if (path[0] != 'E') {
				output->append(path);
				while (!path.empty()) {
					face = path[0];
					num = path[1] - '0';
					solverCube->rotCube(face, num);
					path = path.substr(2);
				}
			}
		}
		if (*solverCube == c)
			return ;
		s->nextPhase();
	}
}

void bfsSolve(Cube *solverCube, Solver *s, std::string *output)
{
	for (int phase = 1; phase <= 4; phase++)
	{
		int i = 0;
		queue<Cube> queue;
		queue.push(*solverCube);
		*solverCube = s->BFS(0, queue);
		solverCube->getColor();
		output->append(solverCube->path);
		// display->waitlist.append(solverCube->path + '+');
		cout << YELLO << "Phase " << phase << " completed: " << endl;
		cout << WHITE << '\t' << translate(solverCube->path) << endl;
		solverCube->path = "";
	}
}

void llamadaASolver (std::string moves){
    if (moves != "") {
        shuffle(moves);
        string output;
        Cube solverCube;

        chrono::milliseconds startTime
        = chrono::duration_cast<chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch());
        solverCube = cube;
        Solver s(solverCube);
        //  bfsSolve(&solverCube, &s, &output);
        hashSolve(&solverCube, &s, &output);
        chrono::milliseconds time
        = chrono::duration_cast<chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()) - startTime;

        std::string mon_output = translate(mergeExtras(output));

        std::cout << GREEN << "Final output:" << endl;
        std::cout << WHITE << mon_output << endl;

        std::cout << "Before solution: " << rubik->moves << std::endl;

        rubik->move(mon_output);

        std::cout << "After solution: " <<  rubik->moves << std::endl;
        
        rubik->moves = "";

        std::cout << "After clear: " << rubik->moves << std::endl;

        std::cout << GREEN << "Total steps: " << WHITE << mergeExtras(output).size() / 2 << endl;
        std::cout << GREEN << "Time to find solution: " << WHITE
        << static_cast<float>(time.count()) / 1000 << " seconds\n";
    }
}










void snake_input(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch (key)
        {
        case GLFW_KEY_RIGHT: snake->move(SNAKE_RIGHT); break;
        case GLFW_KEY_LEFT:  snake->move(SNAKE_LEFT);  break;
        case GLFW_KEY_UP:    snake->move(SNAKE_UP);    break;
        case GLFW_KEY_DOWN:  snake->move(SNAKE_DOWN);  break;
        default: break;
        }
    }
}

void rubik_input(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch (key)
        {
        case GLFW_KEY_1: rubik->move((mods == GLFW_MOD_SHIFT)?"F\'":"F"); break;
        case GLFW_KEY_2: rubik->move((mods == GLFW_MOD_SHIFT)?"B\'":"B"); break;
        case GLFW_KEY_3: rubik->move((mods == GLFW_MOD_SHIFT)?"L\'":"L"); break;
        case GLFW_KEY_4: rubik->move((mods == GLFW_MOD_SHIFT)?"R\'":"R"); break;
        case GLFW_KEY_5: rubik->move((mods == GLFW_MOD_SHIFT)?"U\'":"U"); break;
        case GLFW_KEY_6: rubik->move((mods == GLFW_MOD_SHIFT)?"D\'":"D"); break;
        case GLFW_KEY_7: llamadaASolver(rubik->moves); break;
        case GLFW_KEY_8: rubik->shuffle(15); break;
        default: break;
        }
    }
}

int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WIN_NAME, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    #ifdef _WIN32
        gladLoadGL(glfwGetProcAddress);
    #endif

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // Shaders ================================================================

    Shader *S = new Shader("shaders/texture.vs", "shaders/texture.fs");

    // Vertex info ============================================================

    rubik = new Rubik(S);
    rubik->move("U");
    do
    {
        rubik->update();
    } while (!(rubik->movements_queue.empty()));
    rubik->toSnake();

    snake = new Snake(rubik);

    // Textures ===============================================================
    unsigned int texture1;

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("resources/rubik.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
  
    // RENDER LOOP ===============================================================
    while (!glfwWindowShouldClose(window) && snake->alive)
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        if (rubik->active)
            glfwSetKeyCallback(window, rubik_input);
        else
            glfwSetKeyCallback(window, snake_input);

        // update
        // -----
        if (rubik->active)
            rubik->update();
        else
            snake->update(&camera, deltaTime);
        
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // activate shader
        S->use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        S->setMat4("projection", projection);

        // camera/view transformation
        mat4 view = camera.GetViewMatrix();
        S->setMat4("view", view);

        // render snake or cube
        if (rubik->active)
            rubik->draw();
        else
            snake->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete S;
    delete snake;
    delete rubik;

    glfwTerminate();
    return 0;
}

// g++ -std=c++17 -O2 -framework Cocoa -framework OpenGL -framework IOKit -lglfw3 -w main.cpp -o main.exe

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(C_UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(C_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(C_RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(C_DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && rubik->active)
        camera.ProcessKeyboard(C_FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && rubik->active)
        camera.ProcessKeyboard(C_BACKWARD, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}