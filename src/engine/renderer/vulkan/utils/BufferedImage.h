#pragma once

namespace VK {
    struct BufferedImage {
        VkDeviceMemory memory;
        VkImage image;
        VkImageView imageView;
        VkExtent3D imageExtent;
        VkFormat imageFormat;
    };
}
