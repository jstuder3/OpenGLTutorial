#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <string>
#include <stdexcept>

#include "Shader.h"

std::string loadShaderSource(const char* filePath);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::string vertexShaderCode = loadShaderSource("vertex_shader.glsl");
const char* vertexShaderSource = vertexShaderCode.c_str();

std::string fragmentShaderCode = loadShaderSource("fragment_shader.glsl");
const char* fragmentShaderSource = fragmentShaderCode.c_str();


int main()
{
    // init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // make a platform-independent window with glfw
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

	// test if we can use glad to load the opengl functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // make opengl viewport
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	// set a callback function that adjusts the viewport when the window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // #################################
	// ########## SHADER SETUP #########
	// #################################

    //create shader program
	Shader shader("vertex_shader.glsl", "fragment_shader.glsl");

    //  #################################
	//  ########## VERTEX DATA ##########
	//  #################################

    // dummy triangle
    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,   // first triangle
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // init a vertex buffer object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // move data to the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	// #################################
	// ########## RENDER LOOP ##########
	// #################################
    bool wireframe = false;
    while (!glfwWindowShouldClose(window))
    {
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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

	    // select which shader to use
        shader.use();
		// bind VAO to use the vertex data
        glBindVertexArray(VAO);

        // float timeValue = (float)glfwGetTime();
        // float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        // int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // actual draw calls
        // glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		// unbind the VAO
        glBindVertexArray(0);
        glUseProgram(0);

        // swap the buffers and check and call events (e.g. window resizing callback)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
    shader.deleteShader();

    glfwTerminate();

    return 0;
}

std::string loadShaderSource(const char* filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        throw std::runtime_error("Could not open shader file");
    }
    return std::string((std::istreambuf_iterator<char>(shaderFile)),
        std::istreambuf_iterator<char>());
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    std::cout << "framebuffer_size_callback; width: " << width << " height: " << height << '\n';
}
void processInput(GLFWwindow* window)
{
    // terminate the window when the escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
