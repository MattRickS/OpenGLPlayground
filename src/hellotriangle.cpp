#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
    for (int i =0; i < numAttrs; i++)
        numValuesPerVertex += attrSizes[i];

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, numValuesPerVertex * sizeof(float) * numVertices, vertexData, GL_STATIC_DRAW);

    int offset = 0;
    for (int i =0; i < numAttrs; i++)
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

    int width = 800;
    int height = 600;

    // Initialize the GLFW window
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
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

    // Shaders
    Shader orangeShader = Shader(
        "src/shaders/SimpleVertexShader.glsl",
        "src/shaders/SimpleFragmentShader.glsl"
    );
    Shader yellowShader = Shader(
        "src/shaders/SimpleVertexShader.glsl",
        "src/shaders/SimpleFragmentShaderYellow.glsl"
    );

    // Prepare Objects
    float vertices_1[] = {
         0.5f,  0.4f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.4f, -0.5f, 0.0f,
    };
    unsigned int indices_1[] = {
        0, 1, 2,
    };
    size_t attrSizes_1[] = {3};
    GLuint VAO_1 = LoadVAO(vertices_1, 6, indices_1, 6, attrSizes_1, 1);

    float vertices_2[] = {
        -0.5f, -0.4f, 0.0f,
        -0.5f,  0.5f, 0.0f,
         0.4f,  0.5f, 0.0f,
    };
    unsigned int indices_2[] = {
        0, 1, 2,
    };
    size_t attrSizes_2[] = {3};
    GLuint VAO_2 = LoadVAO(vertices_2, 6, indices_2, 6, attrSizes_2, 1);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Rendering
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wireframe
        orangeShader.use();
        glBindVertexArray(VAO_1);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        yellowShader.use();
        glBindVertexArray(VAO_2);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);  // Unbinding

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}
