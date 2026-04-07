#include "DescriptorAllocator.h"

#include "../Vulkan.h"
#include "../../../../utils/Logger.h"

void VK::DescriptorAllocator::initPool(VkDevice device, const uint32_t maxSets, std::span<PoolSizeRatio> poolRatios, VkAllocationCallbacks* cb) {
    std::vector<VkDescriptorPoolSize> poolSizes{};

    for (const auto& ratio : poolRatios) {
        poolSizes.push_back(VkDescriptorPoolSize{
            .type = ratio.type,
            .descriptorCount = static_cast<uint32_t>(ratio.ratio) * maxSets
        });
    }

    VkDescriptorPoolCreateInfo info;
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.flags = 0;
    info.maxSets = maxSets;
    info.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    info.pPoolSizes = poolSizes.data();

    VK_CHECK(vkCreateDescriptorPool, device, &info, cb, &this->pool);
}

void VK::DescriptorAllocator::clearDescriptors(VkDevice device) const {
    VK_CHECK(Vulkan::vkResetDescriptorPool, device, this->pool, 0);
}

void VK::DescriptorAllocator::destroyPool(VkDevice device, VkAllocationCallbacks* cb) const {
    Vulkan::vkDestroyDescriptorPool(device, this->pool, cb);
}

VkDescriptorSet VK::DescriptorAllocator::allocate(VkDevice device, VkDescriptorSetLayout layout) const {
    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.descriptorPool = this->pool;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &layout;

    VkDescriptorSet set{};
    VK_CHECK(vkAllocateDescriptorSets, device, &info, &set);

    return set;
}
