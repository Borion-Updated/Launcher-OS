#include "Pipeline.h"

#include "../Vulkan.h"

void VK::Pipeline::destroy(VkDevice device, VkAllocationCallbacks* cb) const {
    Vulkan::vkDestroyPipelineLayout(device, this->layout, cb);
    Vulkan::vkDestroyPipeline(device, this->pipeline, cb);
}
