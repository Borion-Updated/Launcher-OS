#include "MatrixStack.h"

MatrixStack::MatrixStack() {
    this->stack.emplace(1.f);
}

glm::mat4& MatrixStack::push() {
    this->stack.push(this->stack.top());

    return this->stack.top();
}

void MatrixStack::pop() {
    this->stack.pop();
}

glm::mat4& MatrixStack::top() {
    return this->stack.top();
}

void MatrixStack::reset() {
    for (size_t i = 0; i < this->stack.size(); i++) {
        this->stack.pop();
    }

    this->stack.emplace(1.f);
}
