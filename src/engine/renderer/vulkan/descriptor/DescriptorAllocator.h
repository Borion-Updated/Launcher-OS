#pragma once

#include "PoolSizeRatio.h"

namespace VK {
    class DescriptorAllocator {
    public:
        void initPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios, VkAllocationCallbacks* cb);
        void clearDescriptors(VkDevice device) const;
        void destroyPool(VkDevice device, VkAllocationCallbacks* cb) const;

        VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout) const;

    private:
        VkDescriptorPool pool{};
    };
}
