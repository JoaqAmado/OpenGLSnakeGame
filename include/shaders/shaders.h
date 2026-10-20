#ifndef SHADER_H
#define SHADER_H

#include <iostream>


class Shader{
    private:
        unsigned int ID;

    public:
        //Builds and compiles shaders in constructor
        Shader(const char* vertexShaderPath, const char* fragmentShaderPath);

        //Helper functions
        void use();
        void setInt(const std::string &name, int value) const;
        void setBool(const std::string &name, bool value) const;
        void setFloat(const std::string &name, float value) const;
        void checkCompileError(unsigned int &shader, std::string type);
        unsigned int getID();
};

#endif
