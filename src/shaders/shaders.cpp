#include <shaders/shaders.h>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
    //Read the code from the file paths
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vertexFile;
    std::ifstream fragmentFile;

    vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        std::stringstream vShaderBuffer;
        std::stringstream fShaderBuffer;

        vertexFile.open(vertexShaderPath);
        fragmentFile.open(fragmentShaderPath);

        if (!vertexFile.is_open() || !fragmentFile.is_open())
        {
            std::cout << "Failed to open files!\n" << "Vertex Path " << vertexShaderPath << '\n' << "Fragment Path " << fragmentShaderPath << std::endl;
        }

        vShaderBuffer << vertexFile.rdbuf();
        fShaderBuffer << fragmentFile.rdbuf();

        vertexCode = vShaderBuffer.str();
        fragmentCode = fShaderBuffer.str();

        vertexFile.close();
        fragmentFile.close();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "Failed to get source code!\n" << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    //Create shaders, and program.
    unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    checkCompileError(vertexShader, "VERTEX");

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    checkCompileError(fragmentShader, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    checkCompileError(ID, "PROGRAM");
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

void Shader::checkCompileError(unsigned int &shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "Shader failed to compile!\n" << infoLog << std::endl;
        }
    }else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "Program failed to link!\n" << infoLog << std::endl;
        }
    }
}

void Shader::use()
{
    glUseProgram(ID);
}
void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()),value);
}
void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()),value);
}
unsigned int Shader::getID() { return ID; }
