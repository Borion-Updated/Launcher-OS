#pragma once

namespace VK {
    struct PushConstants {
        glm::mat4 worldMatrix;
        VkDeviceAddress vertexBuffer;
    };
}
