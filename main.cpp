#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Include OpenGL and GLM headers
#include "dependente/glew/glew.h"
#include "dependente/glfw/glfw3.h"
#include "dependente/glm/glm.hpp"
#include "dependente/glm/gtc/matrix_transform.hpp"
#include "dependente/glm/gtc/type_ptr.hpp"
#include "Character.h" // Contains Character class
#include "Scene.h" // Contains Scene class
#include "shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Healthbar.h"

// GLFW window
GLFWwindow* window;
int width = 820, height = 820;


// Shader program and texture IDs
GLuint texturesProgramID, backgroundTexture;

// VAO, VBO, and IBO for background
GLuint backgroundVAO, backgroundVBO, backgroundIBO;

// Global main character
Character* mainCharacter;

//All Scenes
Scene* centerScene;
Scene* northScene;
Scene* southScene;
Scene* westScene;
Scene* eastScene;
//Global scene
Scene* scene;

//Player stats

const glm::vec4 health_bar_background_color(0.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 health_bar_color(1.0f, 0.0f, 0.0f, 1.0f);

//Rendering Animations with the same speed on all PCs
float delta_time = 0.0f;
float last_frame_time = 0.0f;
const float moving_speed = .8f;  // Increase speed for more visible movement
const float projectile_speed = 5.0f;



// Callback for window resizing
void window_callback(GLFWwindow* window, int new_width, int new_height) {
    glViewport(0, 0, new_width, new_height);
    width = new_width;
    height = new_height;
}

float x_offset = 0.0f;
float y_offset = 0.0f;


float computeOrientationAngle(int w_key, int a_key, int s_key, int d_key) {
    //angles taken counter clockwise
    if (w_key == GLFW_PRESS && d_key == GLFW_PRESS) {
        return 315.0f; // Up-right
    }
    if (w_key == GLFW_PRESS && a_key == GLFW_PRESS) {
        return 45.0f; // Up-left
    }
    if (s_key == GLFW_PRESS && d_key == GLFW_PRESS) {
        return 225.0f; // Down-right
    }
    if (s_key == GLFW_PRESS && a_key == GLFW_PRESS) {
        return 135.0f; // Down-left
    }
    if (w_key == GLFW_PRESS) {
        return 0.0f; // Up
    }
    if (s_key == GLFW_PRESS) {
        return 180.0f; // Down
    }
    if (d_key == GLFW_PRESS) {
        return 270.0f; // Right
    }
    if (a_key == GLFW_PRESS) {
        return 90.0f; // Left
    }

    // No keys pressed, keep the current orientation
    return -1.0f; // Special case to indicate no change in orientation
}



// Handle player movement input
void handleUserInput(GLFWwindow* window, float currentTime) {

    //Close game when hitting escape
    int escape_key = glfwGetKey(window, GLFW_KEY_ESCAPE);

    if (escape_key == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }

    //Detecting clicks (for shooting)
    int mouse_left_click = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

    if (mouse_left_click == GLFW_PRESS) {

        
    }

    //Moving character
    int d_key = glfwGetKey(window, GLFW_KEY_D);
    int a_key = glfwGetKey(window, GLFW_KEY_A);
    int w_key = glfwGetKey(window, GLFW_KEY_W);
    int s_key = glfwGetKey(window, GLFW_KEY_S);

    // Compute x and y offsets for movement based on keys
    if (d_key == GLFW_PRESS && a_key != GLFW_PRESS) {
        x_offset = moving_speed * delta_time;
        mainCharacter->walking = true;
    }
    else if (a_key == GLFW_PRESS && d_key != GLFW_PRESS) {
        x_offset = -moving_speed * delta_time;
        mainCharacter->walking = true;
    }
    else {
        mainCharacter->walking = false;

        x_offset = 0.0f;
    }

    if (w_key == GLFW_PRESS && s_key != GLFW_PRESS) {
        y_offset = moving_speed * delta_time;
        mainCharacter->walking = true;
    }
    else if (s_key == GLFW_PRESS && w_key != GLFW_PRESS) {
        y_offset = -moving_speed * delta_time;
        mainCharacter->walking = true;
    }
    else {
        y_offset = 0.0f;
    }

    // Update the orientation angle based on user input
    float angle = computeOrientationAngle(w_key, a_key, s_key, d_key);
    if (angle != -1.0f) { // -1.0f indicates no change in orientation
        mainCharacter->lookAngle = angle; // Convert to radians if needed
    }

    // Update mainCharacter's position based on input
    mainCharacter->updatePosition(x_offset, y_offset);
    Scene* tempScene = scene->constrainToBoundaries(*mainCharacter);
    if (scene != tempScene) {

        //Switch scenes
        scene = tempScene;
    }
}


int main(void) {
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    // Create window
    window = glfwCreateWindow(width, height, "Textured Scene with Character and Health Bar", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.");
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Enable sticky keys for input handling
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Set viewport and clear color
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Load shaders and textures
    texturesProgramID = LoadShaders("TexturedObjectVertexShader.vertexshader", "TexturedObjectFragmentShader.fragmentshader");
    GLuint healthbarProgramID = LoadShaders("HealthbarVertexShader.vertexshader", "HealthbarFragmentShader.fragmentshader");


    unsigned int backgroundIndices[] = { 0, 1, 3, 1, 2, 3 };

    unsigned int healthBarIndices[] = { 0, 1, 3, 1, 2, 3 };


    // Create main character (spawns in the south)
    mainCharacter = new Character(CharacterType::Main, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(1.2f, 1.2f, 1.0f), 0.0f, glfwGetTime(), false);

    // Create enemies for each scene
    Character* earthBoss = new Character(CharacterType::EarthBoss, glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(1.2f, 1.2f, 1.0f), 90.0f, glfwGetTime(), false);
    Character* iceBoss = new Character(CharacterType::IceBoss, glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(1.2f, 1.2f, 1.0f), 270.0f, glfwGetTime(), false);
    Character* lavaBoss = new Character(CharacterType::LavaBoss, glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(1.2f, 1.2f, 1.0f), 0.0f, glfwGetTime(), false);

    std::vector<Character> enemies;

    //Initializing all scenes

    // Define normalized boundaries based on the reference:
    // Format: {west, east, south, north}
    float centerBoundaries[4] = { -1.4f, 1.4f, -1.4f, 1.4f };    // Center scene
    float northBoundaries[4] = { -0.9f, 0.9f, -1.4f, 0.9f };     // North scene above center
    float eastBoundaries[4] = { -1.4f, 0.9f, -0.9f, 0.9f };     // East scene to the right of center
    float southBoundaries[4] = { -0.9f, 0.9f, -0.9f, 1.4f };   // South scene below center
    float westBoundaries[4] = { -0.9f, 1.4f, -0.9f, 0.9f };   // West scene to the left of center


    //Define normalized starting points (scene transition teleportation points) for each scene
    // Format: {west, east, south, north}
    float westStartingPoint = 0.9f;
    float eastStartingPoint = -0.9f;
    float southStartingPoint = 0.72;
    float northStartingPoint = -0.72;

    //Enemies vectors:
    std::vector<Character> northEnemies = std::vector<Character>();
    northEnemies.push_back(*lavaBoss);

    std::vector<Character> westEnemies = std::vector<Character>();
    westEnemies.push_back(*earthBoss);

    std::vector<Character> eastEnemies = std::vector<Character>();
    eastEnemies.push_back(*iceBoss);

    // Create five scenes with their respective background images and boundaries
    centerScene = new Scene("assets/scenes/MiddleScene.png", centerBoundaries, std::vector<Character>());
    northScene = new Scene("assets/scenes/NorthScene.png", northBoundaries, northEnemies);
    eastScene = new Scene("assets/scenes/EastScene.png", eastBoundaries, eastEnemies);
    southScene = new Scene("assets/scenes/SouthScene.png", southBoundaries, std::vector<Character>());
    westScene = new Scene("assets/scenes/WestScene.png", westBoundaries, westEnemies);

    // Set neighbors for each scene
    centerScene->setNeighbors(westScene, eastScene, southScene, northScene);
    northScene->setNeighbors(nullptr, nullptr, centerScene, nullptr);  // Only south neighbor
    eastScene->setNeighbors(centerScene, nullptr, nullptr, nullptr);   // Only west neighbor
    southScene->setNeighbors(nullptr, nullptr, nullptr, centerScene);  // Only north neighbor
    westScene->setNeighbors(nullptr, centerScene, nullptr, nullptr);   // Only east neighbor

    //Set starting points for each scene
    centerScene->setStartingPoints(&westStartingPoint, &eastStartingPoint, &southStartingPoint, &northStartingPoint);
    westScene->setStartingPoints(&westStartingPoint, nullptr, nullptr, nullptr);
    eastScene->setStartingPoints(nullptr, &eastStartingPoint, nullptr, nullptr);
    southScene->setStartingPoints(nullptr, nullptr, &southStartingPoint, nullptr);
    northScene->setStartingPoints(nullptr, nullptr, nullptr, &northStartingPoint);


    //Starting scene
    scene = southScene;

    //Healthbar setup
    HealthBar* healthBar = new HealthBar(glm::vec3(-0.9f, 0.9f, 0.0f), glm::vec2(0.5f, 0.05f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    // Update health bar ratio
    healthBar->update(mainCharacter->health / 100.0f);

    // Render health bar
    healthBar->render(healthbarProgramID);



    glfwSetFramebufferSizeCallback(window, window_callback);

    // Main loop
    while (!glfwWindowShouldClose(window)) {

        // Calculate delta time
        float current_frame_time = (float)glfwGetTime();
        delta_time = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;

        // Poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
        glfwSwapInterval(1); // Enable VSync to synchronize with monitor's refresh rate

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Handle player input
        handleUserInput(window, current_frame_time);

        scene->render(texturesProgramID, current_frame_time, *mainCharacter, delta_time);
        mainCharacter->render(texturesProgramID, current_frame_time); // Render the character with the shader program

        // Update health bar ratio
        healthBar->update(mainCharacter->health / 100.0f);

        // Render health bar
        healthBar->render(healthbarProgramID);


    }

    // Cleanup
    // Free the dynamically allocated objects
    delete healthBar;
    delete northScene;
    delete southScene;
    delete eastScene;
    delete westScene;
    delete centerScene;
    delete mainCharacter;

    glDeleteProgram(texturesProgramID);
    glfwTerminate();

    return 0;
}
