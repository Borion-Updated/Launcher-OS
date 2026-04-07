#pragma once

#include "PoolSizeRatio.h"

namespace VK {
    class DescriptorAllocatorGrowable {
    public:
        void init(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
        void clearPools(VkDevice device);
        void destroyPools(VkDevice device);

        VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout, void* pNext = nullptr);

    private:
        static VkDescriptorPool createPool(VkDevice device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

        VkDescriptorPool getPool(VkDevice device);

        std::vector<PoolSizeRatio> ratios{};
        std::vector<VkDescriptorPool> fullPools{};
        std::vector<VkDescriptorPool> readyPools{};
        uint32_t setsPerPool{};
    };
}
