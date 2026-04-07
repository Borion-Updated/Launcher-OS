#pragma once

namespace OGL {
    class ShaderUtil {
    public:
        static GLuint loadShader(const void* data, size_t dataSize, GLenum shaderType);
        static void attachAndLinkProgram(GLuint vertexShaderId, GLuint fragmentShaderId, GLuint programId);
    };
}
