#include "PipelineUtils.h"

#include "../Vulkan.h"
#include "../utils/Init.h"
#include "../utils/PushConstants.h"
#include "../utils/ShaderUtils.h"
#include "PipelineBuilder.h"
#include "../../../../utils/Logger.h"
#include "../utils/FragPushConstants.h"

void VK::PipelineUtils::createPipeline(VkDevice device, VkAllocationCallbacks* cb, const VkFormat imageFormat, const void* vertexShaderData, const size_t vertexShaderSize, const void* fragmentShaderData, const size_t fragmentShaderSize,
                                       VkPipeline* pipeline, VkPipelineLayout* pipelineLayout, const bool blend, const bool depth) {
    VkShaderModule vertShader;
    ShaderUtils::loadShaderModule(vertexShaderData, vertexShaderSize, device, cb, &vertShader);

    VkShaderModule fragShader;
    ShaderUtils::loadShaderModule(fragmentShaderData, fragmentShaderSize, device, cb, &fragShader);

    VkPushConstantRange ranges[2]{};

    ranges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    ranges[0].offset = 0;
    ranges[0].size = sizeof(PushConstants);

    ranges[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    ranges[1].offset = 80;
    ranges[1].size = sizeof(FragPushConstants);

    auto pipelineLayoutInfo = Init::createPipelineLayoutInfo();

    pipelineLayoutInfo.pushConstantRangeCount = 2;
    pipelineLayoutInfo.pPushConstantRanges = ranges;

    VK_CHECK(vkCreatePipelineLayout, device, &pipelineLayoutInfo, nullptr, pipelineLayout);

    PipelineBuilder builder{};

    builder.setPipelineLayout(*pipelineLayout);
    builder.setShaders(vertShader, fragShader);
    builder.setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    builder.setPolygonMode(VK_POLYGON_MODE_FILL);
    builder.setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
    builder.setMultisamplingNone();

    if (blend)
        builder.enableAlphaBlending();
    else
        builder.disableBlending();

    if (depth) {
        builder.enableDepthTest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
        builder.setDepthFormat(VK_FORMAT_D32_SFLOAT);
    }
    else {
        builder.disableDepthTest();
        builder.setDepthFormat(VK_FORMAT_UNDEFINED);
    }

    builder.setColorAttachmentFormat(imageFormat);

    *pipeline = builder.buildPipeline(device);

    Vulkan::vkDestroyShaderModule(device, fragShader, cb);
    Vulkan::vkDestroyShaderModule(device, vertShader, cb);
}

void VK::PipelineUtils::createPipeline(VkDevice device, VkAllocationCallbacks* cb, const VkFormat imageFormat, const void* vertexShaderData, const size_t vertexShaderSize, const void* fragmentShaderData, const size_t fragmentShaderSize,
                                        Pipeline& pipeline, const bool blend, const bool depth) {
    createPipeline(device, cb, imageFormat, vertexShaderData, vertexShaderSize, fragmentShaderData, fragmentShaderSize, &pipeline.pipeline, &pipeline.layout, blend, depth);
}

void VK::PipelineUtils::createSamplerPipeline(VkDevice device, VkAllocationCallbacks* cb, VkFormat imageFormat, const void* vertexShaderData, const size_t vertexShaderSize, const void* fragmentShaderData, const size_t fragmentShaderSize,
                                            VkPipeline* pipeline, VkPipelineLayout* pipelineLayout, VkDescriptorSetLayout set, const bool depth) {
    VkShaderModule vertShader;
    ShaderUtils::loadShaderModule(vertexShaderData, vertexShaderSize, device, cb, &vertShader);

    VkShaderModule fragShader;
    ShaderUtils::loadShaderModule(fragmentShaderData, fragmentShaderSize, device, cb, &fragShader);

    VkPushConstantRange ranges[2]{};

    ranges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    ranges[0].offset = 0;
    ranges[0].size = sizeof(PushConstants);

    ranges[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    ranges[1].offset = 80;
    ranges[1].size = sizeof(FragPushConstants);

    auto pipelineLayoutInfo = Init::createPipelineLayoutInfo();

    pipelineLayoutInfo.pushConstantRangeCount = 2;
    pipelineLayoutInfo.pPushConstantRanges = ranges;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &set;

    VK_CHECK(vkCreatePipelineLayout, device, &pipelineLayoutInfo, nullptr, pipelineLayout);

    PipelineBuilder builder{};

    builder.setPipelineLayout(*pipelineLayout);
    builder.setShaders(vertShader, fragShader);
    builder.setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    builder.setPolygonMode(VK_POLYGON_MODE_FILL);
    builder.setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
    builder.setMultisamplingNone();
    builder.enableAlphaBlending();

    if (depth) {
        builder.enableDepthTest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
        builder.setDepthFormat(VK_FORMAT_D32_SFLOAT);
    }
    else {
        builder.disableDepthTest();
        builder.setDepthFormat(VK_FORMAT_UNDEFINED);
    }

    builder.setColorAttachmentFormat(imageFormat);

    *pipeline = builder.buildPipeline(device);

    Vulkan::vkDestroyShaderModule(device, fragShader, cb);
    Vulkan::vkDestroyShaderModule(device, vertShader, cb);
}

void VK::PipelineUtils::createSamplerPipeline(VkDevice device, VkAllocationCallbacks* cb, const VkFormat imageFormat, const void* vertexShaderData, const size_t vertexShaderSize, const void* fragmentShaderData, const size_t fragmentShaderSize,
    Pipeline& pipeline, VkDescriptorSetLayout set, const bool depth) {
    createSamplerPipeline(device, cb, imageFormat, vertexShaderData, vertexShaderSize, fragmentShaderData, fragmentShaderSize, &pipeline.pipeline, &pipeline.layout, set, depth);
}
