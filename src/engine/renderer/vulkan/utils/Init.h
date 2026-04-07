#pragma once

namespace VK {
    class Init {
    public:
        static VkCommandPoolCreateInfo createCommandPool(uint32_t queueFamilyIndex);
        static VkCommandBufferAllocateInfo createCommandBuffer(VkCommandPool pool);
        static VkCommandBufferBeginInfo createCommandBufferBegin(VkCommandBufferUsageFlags flags = 0);
        static VkCommandBufferSubmitInfo createCommandBufferSubmit(VkCommandBuffer cmd);

        static VkImageViewCreateInfo createImageView(VkImage image, VkFormat imageFormat, VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);

        static VkFenceCreateInfo createFenceInfo(VkFenceCreateFlags flags = 0);

        static VkSemaphoreCreateInfo createSemaphoreInfo(VkSemaphoreCreateFlags flags = 0);
        static VkSemaphoreSubmitInfo createSemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);

        static VkImageSubresourceRange createImageSubresourceRange(VkImageAspectFlags aspectMask);

        static VkSubmitInfo2 createSubmitInfo(const VkCommandBufferSubmitInfo* cmd, const VkSemaphoreSubmitInfo* signalSemaphoreInfo, const VkSemaphoreSubmitInfo* waitSemaphoreInfo);

        static VkPipelineLayoutCreateInfo createPipelineLayoutInfo();
        static VkPipelineShaderStageCreateInfo createPipelineShaderStageInfo(VkShaderStageFlagBits stage, VkShaderModule shader, const char* entry = "main");

        static VkRenderingAttachmentInfo createRenderingAttachmentInfo(VkImageView view, const VkClearValue* clear, VkImageLayout layout);
        static VkRenderingAttachmentInfo createDepthAttachmentInfo(VkImageView view, VkImageLayout layout);

        static VkRenderingInfo createRenderingInfo(const VkExtent2D& renderExtent, const VkRenderingAttachmentInfo* colorAttachment, const VkRenderingAttachmentInfo* depthAttachment);
    };
}
