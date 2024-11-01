#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>

#include "stb_image.h"

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
unsigned int loadTexture(const char* path);

unsigned int scr_width = 800;
unsigned int scr_height = 800;

float deltaTime = 0.01f;
float lastFrame = 0.0f;

bool wireframe = false;
float lastWireFrameToggle = 0.0f;
float wireFrameToggleCooldown = 0.2f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = scr_width / 2.0f;
float lastY = scr_height / 2.0f;
bool firstMouse = true;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightDir(-0.2f, -1.0f, -0.3f);

int main()
{
    // init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    Assimp::Importer b;

    // make a platform-independent window with glfw
    GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // callbacks
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// test if we can use glad to load the opengl functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // make opengl viewport
    glViewport(0, 0, scr_width, scr_height);
    glEnable(GL_DEPTH_TEST);

    // #################################
	// ########## SHADER SETUP #########
	// #################################

	Shader shader("vertex_shader.glsl", "fragment_shader.glsl");
    shader.use();

    //  #################################
	//  ########## VERTEX DATA ##########
	//  #################################

    Model ourModel("resources/backpack/backpack.obj");

    // #################################
	// ######### Light Sources #########
    // #################################

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };
    shader.use();

    // directional light
    shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    //point lights
    for (int i = 0; i < 4; i++) {
        shader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
        shader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
        shader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
        shader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
    }

    // spotLight
    shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", 0.09f);
    shader.setFloat("spotLight.quadratic", 0.032f);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    // #################################
	// ######### Transforms ############
    // #################################

    // #################################
    // ######### TEXTURE SETUP #########
    // #################################

	// #################################
	// ########## RENDER LOOP ##########
	// #################################
    while (!glfwWindowShouldClose(window))
    {
        // update window title based on deltatime
        float fps = 1.0f / deltaTime;
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(1) << fps;
        std::string title = "OpenGLTutorial - FPS: " + stream.str();
        glfwSetWindowTitle(window, title.c_str());

        // set deltatime
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // input
        processInput(window);

        // rendering commands here

        // clear the screen and set it to some color
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    // select which shader to use
        shader.use();
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		shader.setFloat("material.shininess", 32.f);

        shader.setVec3("spotLight.position", camera.Position);
        shader.setVec3("spotLight.direction", camera.Front);

        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), ((float)scr_width / (float)scr_height), 0.1f, 100.f);
        shader.setMat4("projection", projection);

        glm::mat4 model;
        // transforms

        model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        ourModel.Draw(shader);

        // swap the buffers and check and call events (e.g. window resizing callback)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shader.deleteShader();
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    scr_width = width;
    scr_height = height;
    std::cout << "framebuffer_size_callback; width: " << width << " height: " << height << '\n';
}

void processInput(GLFWwindow* window)
{
    // terminate the window when the escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
	// toggle wireframe mode
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && ((float)glfwGetTime() > lastWireFrameToggle + wireFrameToggleCooldown)) {
		lastWireFrameToggle = (float)glfwGetTime();
		wireframe = !wireframe;
	}

    //movement input
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
	    // pause execution until the resume button is pressed
        std::cout << "Paused; Press O to resume" << '\n';
        while (glfwGetKey(window, GLFW_KEY_O) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
            glfwPollEvents();
        }
        std::cout << "Resuming!" << '\n';
    }
}

void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn) {
	float xPos = static_cast<float>(xPosIn);
	float yPos = static_cast<float>(yPosIn);
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
        return;
    }
    
    float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    camera.ProcessMouseScroll(static_cast<float>(yOffset));
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
