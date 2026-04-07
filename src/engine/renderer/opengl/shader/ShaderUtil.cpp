#include "ShaderUtil.h"

#include "../../../../utils/Logger.h"

GLuint OGL::ShaderUtil::loadShader(const void* data, const size_t dataSize, const GLenum shaderType) {
    const auto shaderId = glCreateShader(shaderType);

    const auto source = static_cast<const GLchar*>(data);
    const auto length = static_cast<GLint>(dataSize);

    glShaderSource(shaderId, 1, &source, &length);
    glCompileShader(shaderId);

    GLsizei l = 0;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &l);

    if (l > 0) {
        std::vector<GLchar> infoLog(length);
        glGetShaderInfoLog(shaderId, l, &l, infoLog.data());

        logF("Failed to compile shader: {}", infoLog.data());
        std::cerr << "Failed to compile shader: " << infoLog.data() << std::endl;
    }

    return shaderId;
}

void OGL::ShaderUtil::attachAndLinkProgram(const GLuint vertexShaderId, const GLuint fragmentShaderId, const GLuint programId) {
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

    GLsizei length = 0;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &length);

    if (length > 0) {
        std::vector<GLchar> infoLog(length);
        glGetProgramInfoLog(programId, length, &length, infoLog.data());

        logF("Failed to link program: {}", infoLog.data());
        std::cerr << "Failed to link program: " << infoLog.data() << std::endl;
    }
}
