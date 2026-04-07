#include "DescriptorAllocatorGrowable.h"

#include "../Vulkan.h"
#include "../../../../utils/Logger.h"

void VK::DescriptorAllocatorGrowable::init(VkDevice device, const uint32_t maxSets, const std::span<PoolSizeRatio> poolRatios) {
    this->ratios.clear();

    for (const auto& ratio : poolRatios) {
        this->ratios.push_back(ratio);
    }

    const VkDescriptorPool newPool = this->createPool(device, maxSets, poolRatios);

    this->setsPerPool = std::min(static_cast<uint32_t>(static_cast<float>(maxSets) * 1.5f), 4092U);

    this->readyPools.push_back(newPool);
}

void VK::DescriptorAllocatorGrowable::clearPools(VkDevice device) {
    for (const auto pool : this->readyPools) {
        Vulkan::vkResetDescriptorPool(device, pool, 0);
    }

    for (const auto pool : this->fullPools) {
        Vulkan::vkResetDescriptorPool(device, pool, 0);
        this->readyPools.push_back(pool);
    }

    this->fullPools.clear();
}

void VK::DescriptorAllocatorGrowable::destroyPools(VkDevice device) {
    for (const auto pool : this->readyPools) {
        Vulkan::vkDestroyDescriptorPool(device, pool, nullptr);
    }

    this->readyPools.clear();

    for (const auto pool : this->fullPools) {
        Vulkan::vkDestroyDescriptorPool(device, pool, nullptr);
    }

    this->fullPools.clear();
}

VkDescriptorSet VK::DescriptorAllocatorGrowable::allocate(VkDevice device, VkDescriptorSetLayout layout, void* pNext) {
    VkDescriptorPool poolToUse = this->getPool(device);

    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.pNext = pNext;

    info.descriptorPool = poolToUse;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &layout;

    VkDescriptorSet set{};
    const auto res = Vulkan::vkAllocateDescriptorSets(device, &info, &set);

    if (res == VK_ERROR_OUT_OF_POOL_MEMORY || res == VK_ERROR_FRAGMENTED_POOL) {
        this->fullPools.push_back(poolToUse);

        poolToUse = this->getPool(device);

        info.descriptorPool = poolToUse;

        VK_CHECK(vkAllocateDescriptorSets, device, &info, &set);
    }

    this->readyPools.push_back(poolToUse);

    return set;
}

VkDescriptorPool VK::DescriptorAllocatorGrowable::getPool(VkDevice device) {
    VkDescriptorPool newPool;

    if (!this->readyPools.empty()) {
        newPool = this->readyPools.back();
        this->readyPools.pop_back();
    }
    else {
        newPool = this->createPool(device, this->setsPerPool, this->ratios);

        this->setsPerPool = std::min(static_cast<uint32_t>(static_cast<float>(this->setsPerPool) * 1.5f), 4092U);
    }

    return newPool;
}

VkDescriptorPool VK::DescriptorAllocatorGrowable::createPool(VkDevice device, const uint32_t setCount, const std::span<PoolSizeRatio> poolRatios) {
    std::vector<VkDescriptorPoolSize> sizes{};

    for (const auto& ratio : poolRatios) {
        sizes.push_back(VkDescriptorPoolSize{
            .type = ratio.type,
            .descriptorCount = static_cast<uint32_t>(ratio.ratio * static_cast<float>(setCount))
        });
    }

    VkDescriptorPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.flags = 0;
    info.maxSets = setCount;
    info.poolSizeCount = static_cast<uint32_t>(sizes.size());
    info.pPoolSizes = sizes.data();

    VkDescriptorPool newPool;
    VK_CHECK(vkCreateDescriptorPool, device, &info, nullptr, &newPool);
    return newPool;
}
