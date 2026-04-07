#pragma once

#include "Buffer.h"

namespace VK {
    class BufferUtils {
    public:
        static void createBuffer(VkDevice device, VkAllocationCallbacks* cb, size_t size, VkBufferUsageFlags flags, VkMemoryPropertyFlags memoryFlags, const VkPhysicalDeviceMemoryProperties& properties, Buffer& buffer);
        static void createBuffer(VkDevice device, VkAllocationCallbacks* cb, size_t size, VkBufferUsageFlags flags, VkMemoryPropertyFlags memoryFlags, const VkPhysicalDeviceMemoryProperties2& properties, Buffer& buffer);
        static void createBuffer(VkDevice device, VkAllocationCallbacks* cb, size_t size, VkBufferUsageFlags flags, VkMemoryPropertyFlags memoryFlags, const VkPhysicalDeviceMemoryProperties& properties, VkBuffer* buffer, VkDeviceMemory* memory);
        static void createBuffer(VkDevice device, VkAllocationCallbacks* cb, size_t size, VkBufferUsageFlags flags, VkMemoryPropertyFlags memoryFlags, const VkPhysicalDeviceMemoryProperties2& properties, VkBuffer* buffer, VkDeviceMemory* memory);

        static void destroyBuffer(VkDevice device, VkAllocationCallbacks* cb, const Buffer& buffer);
    };
}
