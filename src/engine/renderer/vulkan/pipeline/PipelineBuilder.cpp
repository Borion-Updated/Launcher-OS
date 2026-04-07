#include "PipelineBuilder.h"

#include "../Vulkan.h"
#include "../../../../utils/Logger.h"
#include "../utils/Init.h"

VK::PipelineBuilder::PipelineBuilder() {
    this->clear();
}

void VK::PipelineBuilder::clear() {
    this->inputAssembly = {.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    this->rasterizer = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    this->colorBlendAttachment = {};
    this->multisampling = {.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    this->pipelineLayout = {};
    this->depthStencil = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    this->renderInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
    this->shaderStages.clear();
}

void VK::PipelineBuilder::setPipelineLayout(VkPipelineLayout layout) {
    this->pipelineLayout = layout;
}

void VK::PipelineBuilder::setShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader) {
    this->shaderStages.clear();

    this->shaderStages.push_back(Init::createPipelineShaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader));
    this->shaderStages.push_back(Init::createPipelineShaderStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader));
}

void VK::PipelineBuilder::setInputTopology(const VkPrimitiveTopology topology) {
    this->inputAssembly.topology = topology;
    this->inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void VK::PipelineBuilder::setPolygonMode(const VkPolygonMode polygonMode, const float lineWidth) {
    this->rasterizer.polygonMode = polygonMode;
    this->rasterizer.lineWidth = lineWidth;
}

void VK::PipelineBuilder::setCullMode(const VkCullModeFlags cullMode, const VkFrontFace frontFace) {
    this->rasterizer.cullMode = cullMode;
    this->rasterizer.frontFace = frontFace;
}

void VK::PipelineBuilder::setMultisamplingNone() {
    this->multisampling.sampleShadingEnable = VK_FALSE;
    this->multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    this->multisampling.minSampleShading = 1.f;
    this->multisampling.pSampleMask = nullptr;
    this->multisampling.alphaToCoverageEnable = VK_FALSE;
    this->multisampling.alphaToOneEnable = VK_FALSE;
}

void VK::PipelineBuilder::enableAlphaBlending() {
    this->colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    this->colorBlendAttachment.blendEnable = VK_TRUE;
    this->colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    this->colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    this->colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    this->colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    this->colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    this->colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

void VK::PipelineBuilder::disableBlending() {
    this->colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    this->colorBlendAttachment.blendEnable = VK_FALSE;
}

void VK::PipelineBuilder::setColorAttachmentFormat(const VkFormat format) {
    this->colorAttachmentFormat = format;

    this->renderInfo.colorAttachmentCount = 1;
    this->renderInfo.pColorAttachmentFormats = &this->colorAttachmentFormat;
}

void VK::PipelineBuilder::setDepthFormat(const VkFormat format) {
    this->renderInfo.depthAttachmentFormat = format;
}

void VK::PipelineBuilder::enableDepthTest(const bool depthWriteEnable, const VkCompareOp op) {
    this->depthStencil.depthTestEnable = VK_TRUE;
    this->depthStencil.depthWriteEnable = depthWriteEnable;
    this->depthStencil.depthCompareOp = op;
    this->depthStencil.depthBoundsTestEnable = VK_FALSE;
    this->depthStencil.stencilTestEnable = VK_FALSE;
    this->depthStencil.front = {};
    this->depthStencil.back = {};
    this->depthStencil.minDepthBounds = 0.f;
    this->depthStencil.maxDepthBounds = 1.f;
}

void VK::PipelineBuilder::disableDepthTest() {
    this->depthStencil.depthTestEnable = VK_FALSE;
    this->depthStencil.depthWriteEnable = VK_FALSE;
    this->depthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
    this->depthStencil.depthBoundsTestEnable = VK_FALSE;
    this->depthStencil.stencilTestEnable = VK_FALSE;
    this->depthStencil.front = {};
    this->depthStencil.back = {};
    this->depthStencil.minDepthBounds = 0.f;
    this->depthStencil.maxDepthBounds = 1.f;
}

VkPipeline VK::PipelineBuilder::buildPipeline(VkDevice device) const {
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;

    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;

    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_CLEAR;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &this->colorBlendAttachment;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = &this->renderInfo;

    pipelineInfo.stageCount = static_cast<uint32_t>(this->shaderStages.size());
    pipelineInfo.pStages = this->shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &this->inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &this->rasterizer;
    pipelineInfo.pMultisampleState = &this->multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &this->depthStencil;
    pipelineInfo.layout = this->pipelineLayout;

    VkDynamicState state[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicInfo{};
    dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicInfo.pDynamicStates = &state[0];
    dynamicInfo.dynamicStateCount = 2;

    pipelineInfo.pDynamicState = &dynamicInfo;

    VkPipeline pipeline;
    if (const auto result = Vulkan::vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline); result != VK_SUCCESS) {
        logF("vkCreateGraphicsPipelines: {}", magic_enum::enum_name(result));
        return VK_NULL_HANDLE;
    }

    return pipeline;
}
