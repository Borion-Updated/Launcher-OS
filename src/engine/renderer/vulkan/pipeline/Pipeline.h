#pragma once

namespace VK {
    class Pipeline {
    public:
        void destroy(VkDevice device, VkAllocationCallbacks* cb) const;

        VkPipeline pipeline{};
        VkPipelineLayout layout{};
    };
}
