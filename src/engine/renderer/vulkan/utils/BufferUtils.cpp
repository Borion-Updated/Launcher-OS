#include "BufferUtils.h"

#include "../Vulkan.h"
#include "../../../../utils/Logger.h"

void VK::BufferUtils::createBuffer(VkDevice device, VkAllocationCallbacks* cb, const size_t size, const VkBufferUsageFlags flags, const VkMemoryPropertyFlags memoryFlags, const VkPhysicalDeviceMemoryProperties& properties, Buffer& buffer) {
    buffer.size = size;
    createBuffer(device, cb, size, flags, memoryFlags, properties, &buffer.buffer, &buffer.memory);
}

void VK::BufferUtils::createBuffer(VkDevice device, VkAllocationCallbacks* cb, size_t size, VkBufferUsageFlags flags, VkMemoryPropertyFlags memoryFlags, const VkPhysicalDeviceMemoryProperties2& properties, Buffer& buffer) {
    createBuffer(device, cb, size, flags, memoryFlags, properties.memoryProperties, buffer);
}

void VK::BufferUtils::createBuffer(VkDevice device, VkAllocationCallbacks* cb, const size_t size, const VkBufferUsageFlags flags, VkMemoryPropertyFlags memoryFlags, const VkPhysicalDeviceMemoryProperties& properties, VkBuffer* buffer, VkDeviceMemory* memory) {
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;

    createInfo.flags = 0;
    createInfo.size = size;
    createInfo.usage = flags;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;

    const auto createBuffer = Vulkan::vkCreateBuffer(device, &createInfo, cb, buffer);

    if (createBuffer != VK_SUCCESS) {
        logF("vkCreateBuffer: {}", magic_enum::enum_name(createBuffer));
        return;
    }

    VkBufferMemoryRequirementsInfo2 requirementsInfo{};
    requirementsInfo.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
    requirementsInfo.pNext = nullptr;

    requirementsInfo.buffer = *buffer;

    VkMemoryRequirements2 requirements{};
    requirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
    requirements.pNext = nullptr;

    Vulkan::vkGetBufferMemoryRequirements2(device, &requirementsInfo, &requirements);

    VkMemoryAllocateFlagsInfo flagsInfo{};
    flagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    flagsInfo.pNext = nullptr;
    flagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
    flagsInfo.deviceMask = 0;

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = (flags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) != 0 ? &flagsInfo : nullptr;

    allocInfo.allocationSize = requirements.memoryRequirements.size;
    allocInfo.memoryTypeIndex = 0;

    for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
        if ((requirements.memoryRequirements.memoryTypeBits >> (i & 0x1F) & 1) != 0 && (properties.memoryTypes[i].propertyFlags & memoryFlags) == memoryFlags) {
            allocInfo.memoryTypeIndex = i;
            break;
        }
    }

    const auto allocMem = Vulkan::vkAllocateMemory(device, &allocInfo, cb, memory);

    if (allocMem != VK_SUCCESS) {
        logF("vkAllocateMemory: {}", magic_enum::enum_name(allocMem));
        Vulkan::vkDestroyBuffer(device, *buffer, cb);

#ifdef _M_IX86
        *buffer = NULL;
#else
        *buffer = nullptr;
#endif
        return;
    }

    VkBindBufferMemoryInfo bindInfo{};
    bindInfo.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO;
    bindInfo.pNext = nullptr;

    bindInfo.buffer = *buffer;
    bindInfo.memory = *memory;
    bindInfo.memoryOffset = 0;

    const auto bindMemory = Vulkan::vkBindBufferMemory2(device, 1, &bindInfo);

    if (bindMemory == VK_SUCCESS)
        return;

    logF("vkBindBufferMemory2: {}", magic_enum::enum_name(bindMemory));

#ifdef _M_IX86
    if (*buffer != NULL) {
        Vulkan::vkDestroyBuffer(device, *buffer, cb);
        *buffer = NULL;
    }

    Vulkan::vkFreeMemory(device, *memory, cb);
    *memory = NULL;
#else
    if (*buffer != nullptr) {
        Vulkan::vkDestroyBuffer(device, *buffer, cb);
        *buffer = nullptr;
    }

    Vulkan::vkFreeMemory(device, *memory, cb);
    *memory = nullptr;
#endif
}

void VK::BufferUtils::createBuffer(VkDevice device, VkAllocationCallbacks* cb, size_t size, VkBufferUsageFlags flags, VkMemoryPropertyFlags memoryFlags, const VkPhysicalDeviceMemoryProperties2& properties, VkBuffer* buffer, VkDeviceMemory* memory) {
    createBuffer(device, cb, size, flags, memoryFlags, properties.memoryProperties, buffer, memory);
}

void VK::BufferUtils::destroyBuffer(VkDevice device, VkAllocationCallbacks* cb, const Buffer& buffer) {
    Vulkan::vkDestroyBuffer(device, buffer.buffer, cb);
    Vulkan::vkFreeMemory(device, buffer.memory, cb);
}
