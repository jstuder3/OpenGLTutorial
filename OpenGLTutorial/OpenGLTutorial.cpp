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
float lastButtonToggle = 0.0f;
float buttonToggleCooldown = 0.2f;

//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
Camera camera(glm::vec3(0.0f, 10.0f, 50.0f));
float lastX = scr_width / 2.0f;
float lastY = scr_height / 2.0f;
bool firstMouse = true;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightDir(-0.2f, -1.0f, -0.3f);

GLFWwindow* window;
bool vSyncEnabled = true;

int main()
{
    // init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //enable/disable vsync; is enabled by default in OpenGL
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

	// enable z-buffer
	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // enable MSAA
    glEnable(GL_MULTISAMPLE);

    //enable gamma correction
    glEnable(GL_FRAMEBUFFER_SRGB);

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


    // #################################
	// ########## SHADER SETUP #########
	// #################################

	// Shader shader("shaders/geometryShaderChapter.vert", "shaders/geometryShaderChapter.geom", "shaders/unlitTextureShader.frag");
    // Shader shader("shaders/geometryShaderChapter.vert", "shaders/unlitTextureShader.frag");
	//shader.use();

    // Shader normalShader("shaders/normalsShader.vert", "shaders/normalsShader.geom", "shaders/normalsShader.frag");
    // normalShader.use();

    Shader shader("shaders/simple_shader.vert", "shaders/fixedLightTextureShader.frag");
    shader.use();

	Shader instanceShader("shaders/instancingShader.vert", "shaders/fixedLightTextureShader.frag");
    instanceShader.use();

    Shader skyboxShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.use();

	//  #################################
	//  ########## VERTEX DATA ##########
	//  #################################

    // Model ourModel("resources/models/backpack/backpack.obj");
    Model planetModel("resources/models/planet/planet.obj");
    Model asteroidModel("resources/models/rock/rock.obj");

    int amount = 1000000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand((float)glfwGetTime());
    float radius = 40.0f;
    float offset = 3.f;

    for(unsigned int i = 0; i < amount; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        //translate along circle in range[-offset, offset];
        float angle = (float)i / (float)amount * 360.f;
        float displacement = (rand() & (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0 - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // random scale
        float scale = (rand() % 20) / 100.0f + 0.05f;
        model = glm::scale(model, glm::vec3(scale));

        //random rotation
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // add to model matrices
        modelMatrices[i] = model;
    }

    std::random_shuffle(&modelMatrices[0], &modelMatrices[amount - 1]);

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    amount = 1000;

    for(unsigned int i = 0; i < asteroidModel.meshes.size(); i++) {
        unsigned int VAO = asteroidModel.meshes[i].VAO;
        glBindVertexArray(VAO);
        std::size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }


    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);

    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // #################################
	// ######### Light Sources #########
    // #################################

    // #################################
	// ######### Transforms ############
    // #################################

    // #################################
    // ######### TEXTURE SETUP #########
    // #################################

    std::vector<std::string> faces = {
    "resources/textures/spaceCubemap/right.png",
    "resources/textures/spaceCubemap/left.png",
    "resources/textures/spaceCubemap/top.png",
    "resources/textures/spaceCubemap/bottom.png",
    "resources/textures/spaceCubemap/front.png",
    "resources/textures/spaceCubemap/back.png"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

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
            ImGui::Text("Press G to toggle mouse capture.");
            ImGui::Text("Press I to toggle wireframe mode");
            ImGui::Text("Press P to pause the game and O to resume.");
            ImGui::Text("Press V to toggle VSync (currently %s)", vSyncEnabled ? "enabled" : "disabled");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / io.Framerate, io.Framerate);
            ImGui::Text("Window properties: Width: %d, Height: %d", scr_width, scr_height);
            ImGui::Text("Camera location: %.3f / %.3f  / %.3f", camera.Position.x, camera.Position.y, camera.Position.z);
            ImGui::Text("Number of asteroids: %d", amount);
            ImGui::SliderInt("Number of asteroids", &amount, 1, 100000, "%d", ImGuiSliderFlags_Logarithmic);
            ImGui::End();
        }

        // set camera & perspective transforms ("update state")
        // glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)scr_width / (float)scr_height, 0.1f, 1000.0f);

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

        glm::vec4 lightDir = glm::vec4(-0.2, -1.0f, -0.3f, 0.0f);
        lightDir = glm::vec4(glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.3f, 0.7f, 0.1f)) * lightDir);

        shader.use();
        shader.setMat4("projection", projection);
	    shader.setMat4("view", camera.GetViewMatrix());
        shader.setVec3("dirLight.direction",glm::vec3(lightDir));
        shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        shader.setVec3("viewPos", camera.Position);

        // shader.setMat4("model", glm::mat4(1.0f));
        // shader.setFloat("time", static_cast<float>(glfwGetTime()));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, -0.1f * (float)glfwGetTime(), glm::vec3(0.3f, 0.7f, 0.1f));
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        shader.setMat4("model", model);
        planetModel.Draw(shader);

        instanceShader.use();
        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", camera.GetViewMatrix());
        instanceShader.setVec3("dirLight.direction", glm::vec3(lightDir));
        // instanceShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        instanceShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        instanceShader.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
        instanceShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        instanceShader.setVec3("viewPos", camera.Position);
        instanceShader.setFloat("time", (float)glfwGetTime());
        instanceShader.setMat4("additionalRotate", glm::rotate(glm::mat4(1.0), -0.1f * ((float)glfwGetTime()), glm::vec3(0.f, 1.f, 0.f)));
        instanceShader.setMat4("additionalRotate2", glm::rotate(glm::mat4(1.0), -1.f * ((float)glfwGetTime()), glm::vec3(0.5f, 0.8f, 0.2f)));

        for(unsigned int i = 0; i < asteroidModel.meshes.size(); i++) {
            glBindVertexArray(asteroidModel.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, asteroidModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
        }

        glDepthMask(GL_FALSE);
        skyboxShader.use();
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
        skyboxShader.setMat4("projection", projection);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        //for(unsigned int i = 0; i < amount; i++) {
        //    shader.setMat4("model", modelMatrices[i]);
        //    asteroidModel.Draw(shader);
        //}

        // ImGui stuff
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // necessary for docking functionality and "pull outside of main window" feature
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
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && ((float)glfwGetTime() > lastButtonToggle + buttonToggleCooldown)) {
		lastButtonToggle = (float)glfwGetTime();
		wireframe = !wireframe;
	}

    //movement input
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera.MovementSpeed = BOOSTSPEED;
    }
    else {
        camera.MovementSpeed = SPEED;
    }

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

    // PAUSE toggle
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
	    // pause execution until the resume button is pressed
        lastButtonToggle = (float)glfwGetTime();
        std::cout << "Paused; Press P to resume" << '\n';
        while(((float)glfwGetTime() <= lastButtonToggle + buttonToggleCooldown)) {
	        //do nothing
        }
        glfwPollEvents(); // update the button state once so it doesn't immediately resume
        while ((glfwGetKey(window, GLFW_KEY_P) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)) {
            glfwPollEvents();
        }
        // quick pause to prevent immediately pausing again
        lastButtonToggle = (float)glfwGetTime();
        while (((float)glfwGetTime() <= lastButtonToggle + buttonToggleCooldown)) {
        	//do nothing
        }
        std::cout << "Resuming!" << '\n';
    }

    // MOUSE CAPTURE toggle
    if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && ((float)glfwGetTime() > lastButtonToggle + buttonToggleCooldown)) {
        lastButtonToggle = (float)glfwGetTime();
        unsigned int cursorMode = glfwGetInputMode(window, GLFW_CURSOR);
        if (cursorMode == GLFW_CURSOR_DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else if(cursorMode == GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    // VSYNC toggle
    if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && ((float)glfwGetTime() > lastButtonToggle + buttonToggleCooldown)) {
        lastButtonToggle = (float)glfwGetTime();
		vSyncEnabled = !vSyncEnabled;
        glfwSwapInterval(vSyncEnabled);
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
