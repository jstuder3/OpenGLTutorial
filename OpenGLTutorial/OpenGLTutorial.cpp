#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include "Shader.h"
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

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

int main()
{
    // init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


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

	Shader lightSourceShader("vertex_shader.glsl", "light_source_fragment_shader.glsl");
    lightSourceShader.use();

    //  #################################
	//  ########## VERTEX DATA ##########
	//  #################################

    // dummy triangle
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f),
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);

    // init a vertex buffer object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // move data to the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
 //   glEnableVertexAttribArray(1);

	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
 //   glEnableVertexAttribArray(2);

    // #################################
	// ######### Light Sources #########
    // #################################

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // #################################
	// ######### Transforms ############
    // #################################



    // #################################
    // ######### TEXTURE SETUP #########
    // #################################

  //  unsigned int texture1, texture2;
  //  glGenTextures(1, &texture1);
  //  glBindTexture(GL_TEXTURE_2D, texture1);
  //  // set texture wrapping and filtering mode on the currently bound texture object
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //  // load and generate the texture
  //  stbi_set_flip_vertically_on_load(true);
  //  int width, height, nrChannels;
  //  unsigned char* data = stbi_load("resources/container.jpg", &width, &height, &nrChannels, 0);

  //  if (data) {
  //      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  //      glGenerateMipmap(GL_TEXTURE_2D); 
  //  }
  //  else {
		//std::cout << "Failed to load texture" << std::endl;
  //  }
  //  stbi_image_free(data);

  //  glGenTextures(1, &texture2);
  //  glBindTexture(GL_TEXTURE_2D, texture2);
  //  // set texture wrapping and filtering mode on the currently bound texture object
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //  // load and generate the texture
  //  data = stbi_load("resources/awesomeface.png", &width, &height, &nrChannels, 0);

  //  if (data) {
  //      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  //      glGenerateMipmap(GL_TEXTURE_2D);
  //  }
  //  else {
  //      std::cout << "Failed to load texture" << std::endl;
  //  }
  //  stbi_image_free(data);

  //  shader.setInt("texture1", 0);
  //  shader.setInt("texture2", 1);

	// #################################
	// ########## RENDER LOOP ##########
	// #################################
    while (!glfwWindowShouldClose(window))
    {
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

        lightPos = glm::vec3(1.2f + static_cast<float>(sin(glfwGetTime()*6)), 1.0f + static_cast<float>(cos(glfwGetTime()*3)), 2.0f);

        // input
        processInput(window);

        // rendering commands here

        // clear the screen and set it to some color
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //      glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture1);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, texture2);


	    // select which shader to use
        shader.use();
        shader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", camera.Position);
		// bind VAO to use the vertex data
        glBindVertexArray(VAO);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), ((float)scr_width / (float)scr_height), 0.1f, 100.f);
        shader.setMat4("projection", projection);

        glm::mat4 model;
        // transforms
        for (int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++) {
            float speed = 0.0f * (i+1);
            model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(speed), glm::vec3(1.0f, 0.3f, 0.5f));// translation);
            shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(lightVAO);
        lightSourceShader.use();
        lightSourceShader.setVec3("lightPos", lightPos);
        model = glm::translate(glm::mat4(1.0f), lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightSourceShader.setMat4("model", model);
        lightSourceShader.setMat4("view", view);
        lightSourceShader.setMat4("projection", projection);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // swap the buffers and check and call events (e.g. window resizing callback)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
    shader.deleteShader();
	lightSourceShader.deleteShader();

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
