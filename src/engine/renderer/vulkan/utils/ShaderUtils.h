#pragma once

namespace VK {
    class ShaderUtils {
    public:
        static void loadShaderModule(const void* data, size_t size, VkDevice device, VkAllocationCallbacks* cb, VkShaderModule* outShaderModule);
    };
}
