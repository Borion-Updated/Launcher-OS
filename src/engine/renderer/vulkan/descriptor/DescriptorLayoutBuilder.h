#pragma once

namespace VK {
    class DescriptorLayoutBuilder {
    public:
        void addBinding(uint32_t binding, VkDescriptorType type);
        void clear();
        VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages, VkAllocationCallbacks* cb, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);

    private:
        std::vector<VkDescriptorSetLayoutBinding> bindings{};
    };
}
