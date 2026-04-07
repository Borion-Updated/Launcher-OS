#pragma once

namespace VK {
    class ImageUtils {
    public:
        static void allocateImage(VkDevice device, VkAllocationCallbacks* cb, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage, const VkPhysicalDeviceMemoryProperties& properties,
            VkImage* image, VkDeviceMemory* memory, uint32_t mipLevels = 1);

        static void transitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout, VkPipelineStageFlags2 srcMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, VkPipelineStageFlags2 dstMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT);
        static void transitionImageToClear(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout);
        static void transitionImageToRendering(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout);
        static void transitionImageToCopying(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
        static void transitionImageToPresent(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout);

        static void copyImageToImage(VkCommandBuffer cmd, VkImage src, VkImage dst, const VkExtent2D& srcSize, const VkExtent2D& dstSize);
    };
}
