#pragma once

namespace VK {
    struct Buffer {
        VkBuffer buffer;
        VkDeviceMemory memory;
        size_t size;
    };
}
