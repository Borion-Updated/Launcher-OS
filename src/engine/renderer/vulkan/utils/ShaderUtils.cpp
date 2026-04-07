#include "ShaderUtils.h"

#include "../Vulkan.h"
#include "../../../../utils/Logger.h"

void VK::ShaderUtils::loadShaderModule(const void* data, const size_t size, VkDevice device, VkAllocationCallbacks* cb, VkShaderModule* outShaderModule) {
    VkShaderModuleCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.pNext = nullptr;

    info.codeSize = size;
    info.pCode = static_cast<const uint32_t*>(data);

    VkShaderModule shaderModule;
    VK_CHECK(vkCreateShaderModule, device, &info, cb, &shaderModule);

    *outShaderModule = shaderModule;
}
