#pragma once

namespace OGL {
    class Shader {
    public:
        void load(const b::EmbedInternal::EmbeddedFile& vertexShaderFile, const b::EmbedInternal::EmbeddedFile& fragmentShaderFile, std::span<std::string> attributes);
        void load(const void* vertexShaderData, size_t vertexShaderSize, const void* fragmentShaderData, size_t fragmentShaderSize, std::span<std::string> attributes);
        void cleanup() const;

        void bind(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, const glm::vec4& shaderColor, GLuint textureId = 0xFFFFFFFF) const;
        void unbind();

    private:
        GLint projectionUniformLocation{};
        GLint viewUniformLocation{};
        GLint modelUniformLocation{};
        GLint textureUniformLocation{};
        GLint shaderColorUniformLocation{};
        GLuint programId{};
    };
}
