#include <iostream>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shaders/shaders.h>
#include <camera/camera.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Screen
constexpr int SCR_WIDTH = 1024;
constexpr int SCR_HEIGHT = 800;

constexpr int GRIDSIZE = 10*3*8*10;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Forward declaration
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double yoffset, double xoffset);

int main ()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize glfw" << std::endl; 
        return -1;
    }
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Snake", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader snakeShaders("../src/shaders/shaderSource/snakeShader.sv", "../src/shaders/shaderSource/snakeShader.sf");

    Camera camera(glm::vec3(0.0f, 0.0f, 600.0f));

    float wallVertices[]
    {

    };

    //Grid logic based off screen size
    int smallestD = std::min(SCR_WIDTH, SCR_HEIGHT);
    float squareSide = smallestD/10.0f;
    float end = smallestD/2.0f;
    float start = -end;
    int index = 0;

    //10 x 10 grid
    std::vector<float> floorVertices(GRIDSIZE);
    for (int i = 0; i<10; i++)
    {
        for (int j = 0; j<10; j++)
        {
            float x1 = start + (j * squareSide);
            float y1 = start + (i * squareSide);
            float x2 = start + ((j + 1) * squareSide);
            float y2 = start + ((i + 1) * squareSide);

            // Triangle 1 (CCW)
            //Bottom left
            floorVertices[index++] = x1; floorVertices[index++] = y1; floorVertices[index++] = 0.0f; floorVertices[index++] = 0.0f; floorVertices[index++] = 0.0f;
            //Top left
            floorVertices[index++] = x1; floorVertices[index++] = y2; floorVertices[index++] = 0.0f; floorVertices[index++] = 0.0f; floorVertices[index++] = 1.0f;
            //Bottom right
            floorVertices[index++] = x2; floorVertices[index++] = y1; floorVertices[index++] = 0.0f; floorVertices[index++] = 1.0f; floorVertices[index++] = 0.0f;

            // Triangle 2 (CCW)
            //Top left
            floorVertices[index++] = x1; floorVertices[index++] = y2; floorVertices[index++] = 0.0f; floorVertices[index++] = 0.0f; floorVertices[index++] = 1.0f;
            //Top right
            floorVertices[index++] = x2; floorVertices[index++] = y2; floorVertices[index++] = 0.0f; floorVertices[index++] = 1.0f; floorVertices[index++] = 1.0f;
            //Bottom right    
            floorVertices[index++] = x2; floorVertices[index++] = y1; floorVertices[index++] = 0.0f; floorVertices[index++] = 1.0f; floorVertices[index++] = 0.0f;
        }
    }
    // //Debugging
    // for (int i = 0; i<GRIDSIZE; i++)
    // {
    //     std::cout<<floorVertices[i]<<",";
    //     if ((i+1)%3 == 0)
    //     {
    //         std::cout<<'\n';
    //     }
    // }
    // std::cout << "Final index: " << index << std::endl;

    //Starting snake vertices
    float snakeVertices[]
    {
        squareSide * 4.0f, squareSide * 4.0f, 1.0f,
        squareSide * 5.0f, squareSide * 4.0f, 1.0f,
        squareSide * 5.0f, squareSide * 5.0f, 1.0f,
        squareSide * 4.0f, squareSide * 5.0f, 1.0f
    };

    unsigned int snakeVerticesIndices []
    {
        0, 1, 2,
        2, 3, 0
    };

    //floorVAO
    unsigned int floorVAO;
    glGenVertexArrays(1, &floorVAO);
    glBindVertexArray(floorVAO);

    //floor vertices VBO
    unsigned int floorVBO;
    glGenBuffers(1, &floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, floorVertices.size() * sizeof(float), floorVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    //Floor Texture
    unsigned int floorTex;
    glGenTextures(1, &floorTex);
    glBindTexture(GL_TEXTURE_2D, floorTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Image loading for textures
    int width, height, nrChannels;
    unsigned char* data = stbi_load("../assets/cracked-cement-graphic-spotted-old_1194-9501.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load image" << std::endl;
    }
    stbi_image_free(data);

    //snake VAO
    unsigned int snakeVAO;
    glGenVertexArrays(1, &snakeVAO);
    glBindVertexArray(snakeVAO);

    //snake vertices VBO
    unsigned int snakeVBO;
    glGenBuffers(1, &snakeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, snakeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(snakeVertices), snakeVertices, GL_DYNAMIC_DRAW);

    //snake EBO
    unsigned int snakeEBO;
    glGenBuffers(1, &snakeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, snakeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(snakeVerticesIndices), snakeVerticesIndices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowUserPointer(window, &camera);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);  
    //Render loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glBindTexture(GL_TEXTURE_2D, floorTex);
        snakeShaders.use();

        glm::mat4 model(1.0f);
        unsigned int modelLoc = glGetUniformLocation(snakeShaders.getID(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 3000.0f);
        unsigned int projLoc = glGetUniformLocation(snakeShaders.getID(), "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = camera.GetViewMatrix();
        unsigned int viewLoc = glGetUniformLocation(snakeShaders.getID(), "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6*100);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (camera)
    {
        camera->ProcessMouseScroll(static_cast<float>(yoffset));
    }
}
