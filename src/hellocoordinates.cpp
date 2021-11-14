#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>

#include "Shader.h"
#include "Texture.hpp"
#include "Camera.hpp"


int windowWidth = 800;
int windowHeight = 600;

bool firstMouse = true;
float lastMouseX, lastMouseY;
float deltaTime, lastFrame = 0.0f;
Camera mainCamera = Camera();


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastMouseX = xpos;
        lastMouseX = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos; // reversed since y-coordinates range from bottom to top
    lastMouseX = xpos;
    lastMouseY = ypos;

    mainCamera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    mainCamera.ProcessMouseScroll(yoffset);
}


void processInput(GLFWwindow *window)
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

GLuint LoadVAO(
	float* vertexData,
	size_t numVertices,
	unsigned int* indices,
	size_t numIndices,
    size_t* attrSizes,
    size_t numAttrs
)
{
	int numValuesPerVertex = 0;
    for (int i = 0; i < numAttrs; i++)
        numValuesPerVertex += attrSizes[i];

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, numValuesPerVertex * sizeof(float) * numVertices, vertexData, GL_STATIC_DRAW);

    int offset = 0;
    for (int i = 0; i < numAttrs; i++)
    {
        glVertexAttribPointer(i, attrSizes[i], GL_FLOAT, GL_FALSE, numValuesPerVertex * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(i);
        offset += attrSizes[i];
    }

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return VAO;
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Initialize the GLFW window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Register callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Textures
    stbi_set_flip_vertically_on_load(true);
    Texture texture1 = Texture("src/textures/woodenContainer.jpg", GL_RGB);
    Texture texture2 = Texture("src/textures/awesomeface.png", GL_RGBA);

    // Shaders
    Shader shader = Shader(
        "src/shaders/ColorVertexShader.glsl",
        "src/shaders/ColorFragmentShader.glsl"
    );
    shader.use();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

    // Prepare Objects
    float vertices[] = {
         // positions          // texture coords
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   // top right (front)
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   // bottom right (front)
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   // bottom left (front)
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   // top left (front)

         0.5f,  0.5f, -0.5f,   1.0f, 0.0f,   // top right (back)
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f,   // bottom right (back)
        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   // bottom left (back)
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,   // top left (back)
    };
    unsigned int indices[] = {
        // front
        0, 1, 3,
        1, 2, 3,
        // back
        7, 6, 4,
        6, 5, 4,
        // top
        4, 0, 7,
        0, 3, 7,
        // bottom
        1, 5, 2,
        5, 6, 2,
        // left
        3, 2, 7,
        2, 6, 7,
        // right
        4, 5, 0,
        5, 1, 0,
    };
    size_t attrSizes[] = {3, 2};
    GLuint VAO = LoadVAO(vertices, 8, indices, 36, attrSizes, 2);

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    // Matrices
    glm::mat4 projection;

    // Options
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Rendering
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wireframe
        texture1.activate(GL_TEXTURE0);
        texture2.activate(GL_TEXTURE1);
        shader.use();

        shader.setMat4("view", mainCamera.GetViewMatrix());
        projection = glm::perspective(glm::radians(mainCamera.Zoom), (float)windowWidth/(float)windowHeight, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        glBindVertexArray(VAO);
        for (int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);  // Unbinding

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Limiting frame rate, does not account for render time
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    glfwTerminate();

    return 0;
}
