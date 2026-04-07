#pragma once

#include "Pipeline.h"

namespace VK {
    class PipelineUtils {
    public:
        static void createPipeline(VkDevice device, VkAllocationCallbacks* cb, VkFormat imageFormat, const void* vertexShaderData, size_t vertexShaderSize, const void* fragmentShaderData, size_t fragmentShaderSize,
            VkPipeline* pipeline, VkPipelineLayout* pipelineLayout, bool blend = true, bool depth = true);
        static void createPipeline(VkDevice device, VkAllocationCallbacks* cb, VkFormat imageFormat, const void* vertexShaderData, size_t vertexShaderSize, const void* fragmentShaderData, size_t fragmentShaderSize,
            Pipeline& pipeline, bool blend = true, bool depth = true);

        static void createSamplerPipeline(VkDevice device, VkAllocationCallbacks* cb, VkFormat imageFormat, const void* vertexShaderData, size_t vertexShaderSize, const void* fragmentShaderData, size_t fragmentShaderSize,
            VkPipeline* pipeline, VkPipelineLayout* pipelineLayout, VkDescriptorSetLayout set, bool depth = true);
        static void createSamplerPipeline(VkDevice device, VkAllocationCallbacks* cb, VkFormat imageFormat, const void* vertexShaderData, size_t vertexShaderSize, const void* fragmentShaderData, size_t fragmentShaderSize,
            Pipeline& pipeline, VkDescriptorSetLayout set, bool depth = true);
    };
}
