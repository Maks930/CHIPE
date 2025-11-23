//
// Created by Maks930 on 08/25/2025.
//

#include "Shader.h"

#include <GL/glew.h>

#include <Files/Files.h>
#include <vector>
#include <stdexcept>

using namespace bde::graphics;

GLuint compileShader(const std::string& code, const uint32_t &type);
GLuint linkShader(const std::vector<GLuint>& codes);

Shader::Shader(const uint32_t &id) :
    m_id(id)
{}

Shader::~Shader() {
    glDeleteShader(m_id);
}

void Shader::use() const {
    glUseProgram(m_id);
}

Shader * Shader::load(const std::string &vert, const std::string &frag) {
    GLuint pid;
    GLuint vertexShader;
    GLuint fragmentShader;

    try {
        std::string vertexCode = bde::utils::files::readFile(vert);
        std::string fragmentCode = bde::utils::files::readFile(frag);


        vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
        fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);
    } catch (const std::exception& e) {
        return nullptr;
    }

    try {
        pid = linkShader({vertexShader, fragmentShader});
    } catch (const std::exception &e) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return nullptr;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return new Shader(pid);

}

Shader * Shader::load_t(const std::string &vert_data, const std::string &frag_data) {
    GLuint pid;
    GLuint vertexShader;
    GLuint fragmentShader;

    try {
        vertexShader = compileShader(vert_data, GL_VERTEX_SHADER);
        fragmentShader = compileShader(frag_data, GL_FRAGMENT_SHADER);
    } catch (const std::exception& e) {
        return nullptr;
    }

    try {
        pid = linkShader({vertexShader, fragmentShader});
    } catch (const std::exception &e) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return nullptr;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return new Shader(pid);

}

GLuint compileShader(const std::string& code, const uint32_t &type) {
    const GLchar* shaderCode = code.c_str();
    GLchar infoLog[512];
    GLint success;

    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        throw std::runtime_error("Shader compilate failed");
    }

    return shader;
}

GLuint linkShader(const std::vector<GLuint>& codes) {
    GLuint program = glCreateProgram();

    for (auto& code : codes) {
        glAttachShader(program, code);
    }

    glLinkProgram(program);

    GLint success;
    GLchar infoLog[512];

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);

        glDeleteProgram(program);
        throw std::runtime_error("Shader link failed");
    }

    return program;

}
