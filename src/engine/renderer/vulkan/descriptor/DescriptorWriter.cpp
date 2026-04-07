#include "DescriptorWriter.h"

#include "../Vulkan.h"

void VK::DescriptorWriter::writeBuffer(const int binding, VkBuffer buffer, const size_t size, const size_t offset, const VkDescriptorType type) {
    const auto& info = this->bufferInfos.emplace_back(VkDescriptorBufferInfo{
        .buffer = buffer,
        .offset = offset,
        .range = size
    });

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;

    write.dstBinding = binding;
    write.dstSet = VK_NULL_HANDLE;
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.pBufferInfo = &info;

    this->writes.push_back(write);
}

void VK::DescriptorWriter::writeImage(const int binding, VkImageView image, VkSampler sampler, const VkImageLayout layout, const VkDescriptorType type) {
    const auto& info = this->imageInfos.emplace_back(VkDescriptorImageInfo{
        .sampler = sampler,
        .imageView = image,
        .imageLayout = layout
    });

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;

    write.dstBinding = binding;
    write.dstSet = VK_NULL_HANDLE;
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.pImageInfo = &info;

    this->writes.push_back(write);
}

void VK::DescriptorWriter::clear() {
    this->imageInfos.clear();
    this->bufferInfos.clear();
    this->writes.clear();
}

void VK::DescriptorWriter::updateSet(VkDevice device, VkDescriptorSet set) {
    for (auto& write : this->writes) {
        write.dstSet = set;
    }

    Vulkan::vkUpdateDescriptorSets(device, static_cast<uint32_t>(this->writes.size()), this->writes.data(), 0, nullptr);
}
