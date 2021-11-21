#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/scene.h>

#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>

#include <mygl/Shader.h>
#include <mygl/Texture.hpp>
#include <mygl/Camera.hpp>
#include <mygl/Model.h>


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
    // stbi_set_flip_vertically_on_load(true);
    // Texture texture1 = Texture("src/textures/woodenContainer.jpg", GL_RGB);
    // Texture texture2 = Texture("src/textures/awesomeface.png", GL_RGBA);

    // Shaders
    Shader basicShader = Shader(
        "src/shaders/SimpleVertexShader.glsl",
        "src/shaders/BasicFragmentShader.glsl"
    );
    basicShader.use();

    Shader lightShader = Shader(
        "src/shaders/SimpleVertexShader.glsl",
        "src/shaders/LightFragmentShader.glsl"
    );
    lightShader.use();
    lightShader.setFloat3("lightColor",  1.0f, 1.0f, 1.0f);

    // Prepare Objects
    // Model backpack("../resources/models/backpack/backpack.obj");
    Model backpack("/home/mshaw/git/opengl/resources/models/backpack/backpack.obj");
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
    GLuint cubeVAO = LoadVAO(vertices, 8, indices, 36, attrSizes, 2);
    GLuint lightVAO = LoadVAO(vertices, 8, indices, 36, attrSizes, 2);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    // Matrices
    glm::mat4 projection, lightModel, view;
    lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));
    glm::mat4 cubeModel = glm::mat4(1.0f);

    // Options
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Background
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Rendering
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wireframe
        // texture1.activate(GL_TEXTURE0);
        // texture2.activate(GL_TEXTURE1);

        view = mainCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(mainCamera.Zoom), (float)windowWidth/(float)windowHeight, 0.1f, 100.0f);

        glBindVertexArray(cubeVAO);
        basicShader.use();
        basicShader.setMat4("model", cubeModel);
        basicShader.setMat4("view", view);
        basicShader.setMat4("projection", projection);
        // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        backpack.Draw(basicShader);

        glBindVertexArray(lightVAO);
        lightShader.use();
        lightShader.setMat4("model", lightModel);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);  // Unbinding

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Limiting frame rate, does not account for render time
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    glfwTerminate();

    return 0;
}
