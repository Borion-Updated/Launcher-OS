#pragma once

class MatrixStack {
public:
    MatrixStack();

    glm::mat4& push();
    void pop();

    glm::mat4& top();

    void reset();

private:
    std::stack<glm::mat4> stack{};
};
