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

// GLFW window
GLFWwindow* window;
const int width = 1024, height = 1024;

// Global variables for player stats and movement
//glm::vec3 character_scaling(0.12f, 0.36f, 1.0f);  // Adjust scale if needed
//glm::vec3 player_position(0.0f, -0.5f, 0.0f);    // Starting position

float delta_time = 0.0f;
float last_frame_time = 0.0f;
const float moving_speed = 2.0f;  // Increase speed for more visible movement

// Shader program and texture IDs
GLuint programID, backgroundTexture;

// VAO, VBO, and IBO for background
GLuint backgroundVAO, backgroundVBO, backgroundIBO;

// Global main character
Character* mainCharacter;

//Global scene
Scene* scene;

//Player stats

const glm::vec4 health_bar_background_color(0.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 health_bar_color(1.0f, 0.0f, 0.0f, 1.0f);

int player_health = 50;

// Function to load textures
GLuint loadTexture(const char* filePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int texWidth, texHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filePath, &texWidth, &texHeight, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture: " << filePath << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

// Callback for window resizing
void window_callback(GLFWwindow* window, int new_width, int new_height) {
    glViewport(0, 0, new_width, new_height);
}

float x_offset = 0.0f;
float y_offset = 0.0f;

// Handle player movement input
void handleKeyInput(GLFWwindow* window, Character* mainCharacter, Scene* scene) {
    GLuint d_key = glfwGetKey(window, GLFW_KEY_D);
    GLuint a_key = glfwGetKey(window, GLFW_KEY_A);
    GLuint w_key = glfwGetKey(window, GLFW_KEY_W);
    GLuint s_key = glfwGetKey(window, GLFW_KEY_S);

    // Move left and right
    if (d_key == GLFW_PRESS && a_key != GLFW_PRESS) {
        x_offset = moving_speed * delta_time;
    }
    else if (a_key == GLFW_PRESS && d_key != GLFW_PRESS) {
        x_offset = -moving_speed * delta_time;
    }
    else {
        x_offset = 0.0f;
    }

    // Move up or down
    if (w_key == GLFW_PRESS && s_key != GLFW_PRESS) {
        y_offset = moving_speed * delta_time;
    }
    else if (s_key == GLFW_PRESS && w_key != GLFW_PRESS) {
        y_offset = -moving_speed * delta_time;
    }
    else {
        y_offset = 0.0f;
    }

    // Update mainCharacter's position based on input
    mainCharacter->updatePosition(x_offset, y_offset);
    /*mainCharacter->position.x += x_offset;
    mainCharacter->position.y += y_offset;*/

    scene->constrainToBoundaries(*mainCharacter);
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

    // Load shaders and textures
    programID = LoadShaders("TexturedObjectVertexShader.vertexshader", "TexturedObjectFragmentShader.fragmentshader");
    GLuint healthBarProgramID = LoadShaders("HealthBarVertexShader.vertexshader", "HealthBarFragmentShader.fragmentshader");
    //backgroundTexture = loadTexture("assets/MiddleScene.png");

    // Define boundaries for the scene
    float boundaries[4] = { -0.9f, 0.9f, -0.72f, 0.72f };

    // Define background vertices and indices
    //float backgroundVertices[] = {
    //    1.0f,  1.0f, 0.0f, 1.0f, 1.0f,  // top right
    //    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
    //   -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom left
    //   -1.0f,  1.0f, 0.0f, 0.0f, 1.0f   // top left
    //};

    unsigned int backgroundIndices[] = { 0, 1, 3, 1, 2, 3 };

    unsigned int healthBarIndices[] = { 0, 1, 3, 1, 2, 3 };

    // Setup VAOs, VBOs, and IBOs for background
    /*glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);
    glGenBuffers(1, &backgroundIBO);

    glBindVertexArray(backgroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backgroundIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backgroundIndices), backgroundIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);*/

    // Initialize the main character
    mainCharacter = new Character("assets/main_character/MainHoldingGun.png", 50, 10, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(0.12f, 0.36f, 1.0f));
    /*std::vector<Character> enemies = {
        Character("assets/EnemyCharacter.png", 30, 5, glm::vec3(-0.3f, 0.3f, 0.0f), glm::vec3(0.12f, 0.36f, 1.0f)),
        Character("assets/EnemyCharacter.png", 30, 5, glm::vec3(0.3f, -0.3f, 0.0f), glm::vec3(0.12f, 0.36f, 1.0f))
    };*/
    std::vector<Character> enemies;

    // Initialize the global scene with the background image, boundaries, main character, and enemies
    scene = new Scene("assets/scenes/MiddleScene.png", boundaries, enemies);

    // Health Bar Background setup
    GLuint healthBarBackgroundVAO, healthBarBackgroundVBO, healthBarBackgroundIBO;
    glGenVertexArrays(1, &healthBarBackgroundVAO);
    glGenBuffers(1, &healthBarBackgroundVBO);
    glGenBuffers(1, &healthBarBackgroundIBO);
    float healthBarBackgroundVertices[] = {
        -0.95f, 0.9f, 0.0f,  // top left
        -0.95f, 0.85f, 0.0f, // bottom left
        -0.45f, 0.85f, 0.0f, // bottom right
        -0.45f, 0.9f, 0.0f   // top right
    };

    // Bind and buffer health bar background
    glBindVertexArray(healthBarBackgroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, healthBarBackgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(healthBarBackgroundVertices), healthBarBackgroundVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, healthBarBackgroundIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backgroundIndices), backgroundIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Health Bar Foreground (Red) setup
    GLuint healthBarForegroundVAO, healthBarForegroundVBO, healthBarForegroundIBO;
    glGenVertexArrays(1, &healthBarForegroundVAO);
    glGenBuffers(1, &healthBarForegroundVBO);
    glGenBuffers(1, &healthBarForegroundIBO);
    float healthBarForegroundVertices[] = {
        -0.95f, 0.89f, 0.0f,
        -0.95f, 0.86f, 0.0f,
        -0.45f, 0.86f, 0.0f,
        -0.45f, 0.89f, 0.0f
    };

    // Bind and buffer health bar foreground
    glBindVertexArray(healthBarForegroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, healthBarForegroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(healthBarForegroundVertices), healthBarForegroundVertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, healthBarForegroundIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(healthBarIndices), healthBarIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glfwSetFramebufferSizeCallback(window, window_callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Main loop
    while (!glfwWindowShouldClose(window)) {

        // Calculate delta time
        float current_frame_time = glfwGetTime();
        delta_time = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;

        // Poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Handle player input
        handleKeyInput(window, mainCharacter, scene);

        // Render Background
        /*glUseProgram(programID);
        glUniform1i(glGetUniformLocation(programID, "texture1"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glUniformMatrix4fv(glGetUniformLocation(programID, "transform"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glBindVertexArray(backgroundVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/

        // Render the Main Character with updated position
        //glm::mat4 characterTransform = glm::translate(glm::mat4(1.0f), player_position);
        //characterTransform = glm::scale(characterTransform, character_scaling);

        // Apply transformation matrix for the character
        //glUniformMatrix4fv(glGetUniformLocation(programID, "transform"), 1, GL_FALSE, glm::value_ptr(characterTransform));
        scene->render(programID);
        mainCharacter->render(programID); // Render the character with the shader program
        // Render Health Bar Background (black rectangle)
        glUseProgram(healthBarProgramID);
        unsigned int health_bar_color_loc = glGetUniformLocation(healthBarProgramID, "healthBarColor");

        // Render health bar background
        glUseProgram(healthBarProgramID);
        glUniform4fv(health_bar_color_loc, 1, glm::value_ptr(health_bar_background_color));
        glBindVertexArray(healthBarBackgroundVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Update and render health bar foreground
        float health_ratio = player_health / 100.0f;
        healthBarForegroundVertices[6] = -0.95f + health_ratio * 0.5f;  // Dynamic width
        healthBarForegroundVertices[9] = -0.95f + health_ratio * 0.5f;  // Dynamic width

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(healthBarForegroundVertices), healthBarForegroundVertices);
        glUniform4fv(health_bar_color_loc, 1, glm::value_ptr(health_bar_color));
        glBindVertexArray(healthBarForegroundVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Unbind the health bar VAO after rendering both background and foreground
        glBindVertexArray(0);
    }

    // Cleanup
    // Free the dynamically allocated main character

    scene->cleanup();
    mainCharacter->cleanup();

    delete scene;
    delete mainCharacter;
    glDeleteBuffers(1, &healthBarBackgroundVBO);
    glDeleteBuffers(1, &healthBarForegroundVBO);
    glDeleteBuffers(1, &healthBarBackgroundIBO);
    glDeleteBuffers(1, &healthBarForegroundIBO);
    glDeleteVertexArrays(1, &healthBarBackgroundVAO);
    glDeleteVertexArrays(1, &healthBarForegroundVAO);

    glDeleteProgram(programID);
    glfwTerminate();

    return 0;
}
