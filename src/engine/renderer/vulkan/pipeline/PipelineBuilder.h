#pragma once

namespace VK {
    class PipelineBuilder {
    public:
        PipelineBuilder();

        void clear();

        void setPipelineLayout(VkPipelineLayout layout);
        void setShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
        void setInputTopology(VkPrimitiveTopology topology);
        void setPolygonMode(VkPolygonMode polygonMode, float lineWidth = 1.f);
        void setCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
        void setMultisamplingNone();
        void enableAlphaBlending();
        void disableBlending();
        void setColorAttachmentFormat(VkFormat format);
        void setDepthFormat(VkFormat format);
        void enableDepthTest(bool depthWriteEnable, VkCompareOp op);
        void disableDepthTest();

        VkPipeline buildPipeline(VkDevice device) const;

    private:
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineMultisampleStateCreateInfo multisampling{};
        VkPipelineLayout pipelineLayout{};
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        VkPipelineRenderingCreateInfo renderInfo{};
        VkFormat colorAttachmentFormat{};
    };
}
