#include "Shader.h"

#include "ShaderUtil.h"

void OGL::Shader::load(const b::EmbedInternal::EmbeddedFile& vertexShaderFile, const b::EmbedInternal::EmbeddedFile& fragmentShaderFile, const std::span<std::string> attributes) {
    this->load(vertexShaderFile.data(), vertexShaderFile.size(), fragmentShaderFile.data(), fragmentShaderFile.size(), attributes);
}

void OGL::Shader::load(const void* vertexShaderData, const size_t vertexShaderSize, const void* fragmentShaderData, const size_t fragmentShaderSize, const std::span<std::string> attributes) {
    const auto vertexShader = ShaderUtil::loadShader(vertexShaderData, vertexShaderSize, GL_VERTEX_SHADER);
    const auto fragmentShader = ShaderUtil::loadShader(fragmentShaderData, fragmentShaderSize, GL_FRAGMENT_SHADER);

    this->programId = glCreateProgram();

    for (int i = 0; i < attributes.size(); i++) {
        const auto& str = attributes[i];

        glBindAttribLocation(this->programId, i, str.c_str());
    }

    ShaderUtil::attachAndLinkProgram(vertexShader, fragmentShader, this->programId);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(this->programId);

    this->projectionUniformLocation = glGetUniformLocation(this->programId, "projectionMatrix");
    this->viewUniformLocation = glGetUniformLocation(this->programId, "viewMatrix");
    this->modelUniformLocation = glGetUniformLocation(this->programId, "modelMatrix");
    this->textureUniformLocation = glGetUniformLocation(this->programId, "texture_0");
    this->shaderColorUniformLocation = glGetUniformLocation(this->programId, "shaderColor");

    glUseProgram(0);
}

void OGL::Shader::cleanup() const {
    glUseProgram(0);

    glDeleteProgram(this->programId);
}

void OGL::Shader::bind(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, const glm::vec4& shaderColor, const GLuint textureId) const {
    glUseProgram(this->programId);

    glUniformMatrix4fv(this->projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(this->viewUniformLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(this->modelUniformLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4fv(this->shaderColorUniformLocation, 1, glm::value_ptr(shaderColor));

    if (textureId != 0xFFFFFFFF && this->textureUniformLocation != -1) {
        glUniform1i(this->textureUniformLocation, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
    }
}

void OGL::Shader::unbind() {
    glUseProgram(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
