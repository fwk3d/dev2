#pragma once
#include <stdexcept>

class Shader
{
    GLuint id;

public:
    Shader (const GLchar* src, GLsizei length, GLenum type);
    ~Shader ();

    void compile (void);
    GLuint getId ();

    static Shader* load_from_file (const GLchar* file_name, GLenum type);
};

class ShaderProgram
{
    GLuint id;

public:
    ShaderProgram (Shader** shaders, GLuint num);
    ~ShaderProgram ();
    void use ();
    GLuint getId ();
};

class ShaderError: public std::runtime_error
{
public:
    ShaderError(const char* message): std::runtime_error(message) {};
};
