#include "DescriptorLayoutBuilder.h"

#include "../Vulkan.h"
#include "../../../../utils/Logger.h"

void VK::DescriptorLayoutBuilder::addBinding(const uint32_t binding, const VkDescriptorType type) {
    VkDescriptorSetLayoutBinding bind{};
    bind.binding = binding;
    bind.descriptorCount = 1;
    bind.descriptorType = type;
    this->bindings.push_back(bind);
}

void VK::DescriptorLayoutBuilder::clear() {
    this->bindings.clear();
}

VkDescriptorSetLayout VK::DescriptorLayoutBuilder::build(VkDevice device, const VkShaderStageFlags shaderStages, VkAllocationCallbacks* cb, void* pNext, const VkDescriptorSetLayoutCreateFlags flags) {
    for (auto& binding : this->bindings) {
        binding.stageFlags |= shaderStages;
    }

    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pNext = pNext;
    info.pBindings = this->bindings.data();
    info.bindingCount = static_cast<uint32_t>(this->bindings.size());
    info.flags = flags;

    VkDescriptorSetLayout set{};
    VK_CHECK(vkCreateDescriptorSetLayout, device, &info, cb, &set);

    return set;
}
