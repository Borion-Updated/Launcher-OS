#include "ImageUtils.h"

#include "../Vulkan.h"
#include "Init.h"
#include "../../../../utils/Logger.h"

void VK::ImageUtils::allocateImage(VkDevice device, VkAllocationCallbacks* cb, const uint32_t width, const uint32_t height, const VkFormat format, const VkSampleCountFlagBits samples, const VkImageUsageFlags usage,
                                   const VkPhysicalDeviceMemoryProperties& properties, VkImage* image, VkDeviceMemory* memory, const uint32_t mipLevels) {
    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.pNext = nullptr;

    createInfo.flags = 0;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.format = format;
    createInfo.extent = VkExtent3D{width, height, 1};
    createInfo.mipLevels = mipLevels;
    createInfo.arrayLayers = 1;
    createInfo.samples = samples;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    const auto createImage = Vulkan::vkCreateImage(device, &createInfo, cb, image);

    if (createImage != VK_SUCCESS) {
        logF("vkCreateImage: {}", magic_enum::enum_name(createImage));
        return;
    }

    VkImageMemoryRequirementsInfo2 requirementsInfo{};
    requirementsInfo.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
    requirementsInfo.pNext = nullptr;

    requirementsInfo.image = *image;

    VkMemoryRequirements2 requirements{};
    requirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
    requirements.pNext = nullptr;

    Vulkan::vkGetImageMemoryRequirements2(device, &requirementsInfo, &requirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;

    allocInfo.allocationSize = requirements.memoryRequirements.size;
    allocInfo.memoryTypeIndex = 0;

    for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
        if ((requirements.memoryRequirements.memoryTypeBits >> (i & 0x1F) & 1) != 0 && (properties.memoryTypes[i].propertyFlags & 1) != 0) {
            allocInfo.memoryTypeIndex = i;
            break;
        }
    }

    const auto allocMem = Vulkan::vkAllocateMemory(device, &allocInfo, cb, memory);

    if (allocMem != VK_SUCCESS) {
        logF("vkAllocateMemory: {}", magic_enum::enum_name(allocMem));

        Vulkan::vkDestroyImage(device, *image, cb);

#ifdef _M_IX86
        *image = NULL;
#else
        *image = nullptr;
#endif
        return;
    }

    VkBindImageMemoryInfo bindInfo{};
    bindInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
    bindInfo.pNext = nullptr;

    bindInfo.image = *image;
    bindInfo.memory = *memory;
    bindInfo.memoryOffset = 0;

    const auto bindMem = Vulkan::vkBindImageMemory2(device, 1, &bindInfo);

    if (bindMem == VK_SUCCESS)
        return;

    logF("vkBindImageMemory2: {}", magic_enum::enum_name(bindMem));

#ifdef _M_IX86
    if (*image != NULL) {
        Vulkan::vkDestroyImage(device, *image, cb);
        *image = NULL;
    }

    Vulkan::vkFreeMemory(device, *memory, cb);
    *memory = NULL;
#else
    if (*image != nullptr) {
        Vulkan::vkDestroyImage(device, *image, cb);
        *image = nullptr;
    }

    Vulkan::vkFreeMemory(device, *memory, cb);
    *memory = nullptr;
#endif
}

void VK::ImageUtils::transitionImage(VkCommandBuffer cmd, VkImage image, const VkImageLayout currentLayout, const VkImageLayout newLayout, const VkPipelineStageFlags2 srcMask, const VkPipelineStageFlags2 dstMask) {
    VkImageMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;

    barrier.srcStageMask = srcMask;
    barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;

    barrier.dstStageMask = dstMask;
    barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    barrier.oldLayout = currentLayout;
    barrier.newLayout = newLayout;

    barrier.image = image;

    barrier.subresourceRange = Init::createImageSubresourceRange(newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

    VkDependencyInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    info.pNext = nullptr;

    info.dependencyFlags = 0;

    info.memoryBarrierCount = 0;
    info.pMemoryBarriers = nullptr;

    info.bufferMemoryBarrierCount = 0;
    info.pBufferMemoryBarriers = nullptr;

    info.imageMemoryBarrierCount = 1;
    info.pImageMemoryBarriers = &barrier;

    Vulkan::vkCmdPipelineBarrier2(cmd, &info);
}

void VK::ImageUtils::transitionImageToClear(VkCommandBuffer cmd, VkImage image, const VkImageLayout currentLayout) {
    transitionImage(cmd, image, currentLayout, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_2_NONE, VK_PIPELINE_STAGE_2_CLEAR_BIT);
}

void VK::ImageUtils::transitionImageToRendering(VkCommandBuffer cmd, VkImage image, const VkImageLayout currentLayout) {
    transitionImage(cmd, image, currentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_CLEAR_BIT, VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT);
}

void VK::ImageUtils::transitionImageToCopying(VkCommandBuffer cmd, VkImage image, const VkImageLayout currentLayout, const VkImageLayout newLayout) {
    transitionImage(cmd, image, currentLayout, newLayout, VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_2_BLIT_BIT);
}

void VK::ImageUtils::transitionImageToPresent(VkCommandBuffer cmd, VkImage image, const VkImageLayout currentLayout) {
    transitionImage(cmd, image, currentLayout, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_2_BLIT_BIT, VK_PIPELINE_STAGE_2_NONE);
}

void VK::ImageUtils::copyImageToImage(VkCommandBuffer cmd, VkImage src, VkImage dst, const VkExtent2D& srcSize, const VkExtent2D& dstSize) {
    VkImageBlit2 blit{};
    blit.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
    blit.pNext = nullptr;

    blit.srcOffsets[1].x = srcSize.width;
    blit.srcOffsets[1].y = srcSize.height;
    blit.srcOffsets[1].z = 1;

    blit.dstOffsets[1].x = dstSize.width;
    blit.dstOffsets[1].y = dstSize.height;
    blit.dstOffsets[1].z = 1;

    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.srcSubresource.mipLevel = 0;

    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;
    blit.dstSubresource.mipLevel = 0;

    VkBlitImageInfo2 info{};
    info.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
    info.pNext = nullptr;

    info.srcImage = src;
    info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

    info.dstImage = dst;
    info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    info.regionCount = 1;
    info.pRegions = &blit;

    info.filter = VK_FILTER_LINEAR;

    Vulkan::vkCmdBlitImage2(cmd, &info);
}
