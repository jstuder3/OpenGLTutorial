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

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "stb_image.h"

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(std::vector<std::string>& faces);

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

GLFWwindow* window;

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
    window = glfwCreateWindow(scr_width, scr_height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //enable/disable vsync; is enabled by default in OpenGL
    bool vSyncEnabled = true;
    glfwSwapInterval(vSyncEnabled);

    // callbacks
    glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// test if we can use glad to load the opengl functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // make opengl viewport
    glViewport(0, 0, scr_width, scr_height);

    // ImGui initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // enable z-buffer
	glEnable(GL_DEPTH_TEST);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    // #################################
	// ########## SHADER SETUP #########
	// #################################

	Shader shader("shaders/geometryShaderChapter.vert", "shaders/geometryShaderChapter.geom", "shaders/geometryShaderChapter.frag");
    shader.use();

    //  #################################
	//  ########## VERTEX DATA ##########
	//  #################################

    // Model ourModel("resources/backpack/backpack.obj");

    //float cubeVertices[] = {
    //    // Back face
    //    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
    //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    //     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
    //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    //    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
    //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    //    // Front face
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    //     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    //     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    //     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    //    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    //    // Left face
    //    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    //    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    //    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    //    // Right face
    //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
    //     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
    //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
    //     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
    //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
    //     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
    //     // Bottom face
    //     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    //      0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
    //      0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    //      0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    //     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    //     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    //     // Top face
    //     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    //      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    //      0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
    //      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    //     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    //     -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
    //};
    float points[] = {
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
    };


    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // #################################
	// ######### Light Sources #########
    // #################################

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
        // set deltatime
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        // update window title based on deltatime
        float fps = 1.0f / deltaTime;
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(1) << fps;
        std::string title = "OpenGLTutorial - FPS: " + stream.str();
        glfwSetWindowTitle(window,title.c_str());

    	// input
        processInput(window);

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        // ImGui::ShowDemoWindow();

        {
            ImGui::Begin("Info");
            ImGui::Text("Press ESC to terminate");
            ImGui::Text("Press G to un-capture the mouse. Press H to capture it.");
            ImGui::Text("Press I to toggle wireframe mode");
            ImGui::Text("Press P to pause the game and O to resume.");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / io.Framerate, io.Framerate);
            ImGui::Text("Window properties: Width: %d, Height: %d", scr_width, scr_height);
            ImGui::End();
        }


        // set camera & perspective transforms ("update state")
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)scr_width / (float)scr_height, 0.1f, 100.0f);

        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // render
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
 /*       shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("cameraPos", camera.Position);*/

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 4);
        glBindVertexArray(0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        // swap the buffers and check and call events (e.g. window resizing callback)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    shader.deleteShader();
    glfwDestroyWindow(window);
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

    if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
        return;
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
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
        return;
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

        if(format == GL_RGBA) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
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

unsigned int loadCubemap(std::vector<std::string>& faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
