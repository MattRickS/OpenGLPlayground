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
#include <mygl/Lights.h>
#include <mygl/Primitives.hpp>
#include <mygl/TextureCache.hpp>
#include <mygl/Buffers.hpp>


unsigned int windowWidth = 800;
unsigned int windowHeight = 600;

bool firstMouse = true;
float lastMouseX, lastMouseY;
float deltaTime, lastFrame = 0.0f;
Camera mainCamera = Camera();

bool isWireframe = false;


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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        auto mode = isWireframe ? GL_FILL : GL_LINE;
        glPolygonMode(GL_FRONT_AND_BACK, mode);
        isWireframe = !isWireframe;
    }

}


void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
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
    glfwSetKeyCallback(window, key_callback);

    // Lights
    std::vector<PointLight> pointLights = std::vector<PointLight> {
        {
            glm::vec3(0.7f, 0.2f, 2.0f),
            1.0f,
            0.09f,
            0.032f,
            glm::vec3(0.05f),
            glm::vec3(0.8f),
            glm::vec3(1.0f)
        },
        {
            glm::vec3(2.3f, -3.3f, -4.0f),
            1.0f,
            0.09f,
            0.032f,
            glm::vec3(0.05f),
            glm::vec3(0.8f),
            glm::vec3(1.0f)
        },
    };
    DirLight dirLight { glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.05f), glm::vec3(0.4f), glm::vec3(0.5f) };
    SpotLight spotLight
    {
        mainCamera.Position,
        mainCamera.Front,
        12.5f,
        15.0f,
        1.0f,
        0.09f,
        0.032f,
        glm::vec3(0.0f),
        glm::vec3(1.0f),
        glm::vec3(1.0f)
    };

    // Textures
    stbi_set_flip_vertically_on_load(true);
    TextureCache &textureCache = TextureCache::getInstance();
    textureCache.LoadTexture("resources/textures/woodenContainer.jpg");
    TextureID woodenContainerDiffuse = TextureID {
        textureCache.GetTexture("resources/textures/woodenContainer.jpg"), TextureType::diffuse
    };

    // Shaders
    Shader screenShader = Shader(
        "src/shaders/ScreenVertexShader.glsl",
        "src/shaders/ScreenFragmentShader.glsl"
    );
    Shader texturedShader = Shader(
        "src/shaders/BasicVertexShader.glsl",
        "src/shaders/TexturedFragmentShader.glsl"
    );
    Shader basicShader = Shader(
        "src/shaders/BasicVertexShader.glsl",
        "src/shaders/BasicFragmentShader.glsl"
    );

    // Prepare Objects
    Model backpack("/home/mshaw/git/opengl/resources/models/backpack/backpack.obj");
    Cube cube { std::vector<TextureID> {woodenContainerDiffuse} };
    Quad quad;

    // Matrices
    glm::mat4 view;
    glm::mat4 projection = glm::perspective(glm::radians(mainCamera.Zoom), (float)windowWidth/(float)windowHeight, 0.1f, 100.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 cubeModel = glm::translate(model, glm::vec3(4.0f, 4.0f, 4.0f));

    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Shader config
    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    basicShader.use();
    basicShader.setFloat("material.shininess", 32.0f);
    basicShader.setDirLight("dirLight", dirLight);
    basicShader.setSpotLight("spotLight", spotLight);
    for (int i = 0; i < pointLights.size(); i++)
        basicShader.setPointLight("pointLights", pointLights[i], i);

    // Buffers
    FrameBuffer framebuffer;
    framebuffer.use();
    TextureBuffer textureBuffer {windowWidth, windowHeight};
    RenderBuffer renderBuffer {windowWidth, windowHeight};
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Setup Render Buffer
        framebuffer.use();
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw Backpack
        view = mainCamera.GetViewMatrix();
        basicShader.use();
        spotLight.position = mainCamera.Position;
        spotLight.direction = mainCamera.Front;
        basicShader.setSpotLight("spotLight", spotLight);
        basicShader.setVec3("viewPos", mainCamera.Position);
        basicShader.setMat4("model", model);
        basicShader.setMat4("view", view);
        basicShader.setMat4("projection", projection);

        backpack.Draw(basicShader);

        // Draw Cube
        texturedShader.use();
        texturedShader.setMat4("model", cubeModel);
        texturedShader.setMat4("view", view);
        texturedShader.setMat4("projection", projection);

        cube.Draw(texturedShader);

        // Setup Texture Buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render to Texture
        screenShader.use();
        textureBuffer.use();
        quad.Draw(screenShader);

        glBindVertexArray(0);  // Unbinding

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Limiting frame rate, does not account for render time
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    glfwTerminate();

    return 0;
}
