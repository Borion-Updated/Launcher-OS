#include "Vulkan.h"

#include "../../window/Win32Window.h"

static PFN_vkCreateInstance FvkCreateInstance = nullptr;
static PFN_vkDestroyInstance FvkDestroyInstance = nullptr;
static PFN_vkCreateDebugUtilsMessengerEXT FvkCreateDebugUtilsMessengerEXT = nullptr;
static PFN_vkDestroyDebugUtilsMessengerEXT FvkDestroyDebugUtilsMessengerEXT = nullptr;
static PFN_vkGetDeviceProcAddr FvkGetDeviceProcAddr = nullptr;
static PFN_vkEnumeratePhysicalDevices FvkEnumeratePhysicalDevices = nullptr;
static PFN_vkGetPhysicalDeviceProperties FvkGetPhysicalDeviceProperties = nullptr;
static PFN_vkGetPhysicalDeviceProperties2 FvkGetPhysicalDeviceProperties2 = nullptr;
static PFN_vkEnumerateDeviceExtensionProperties FvkEnumerateDeviceExtensionProperties = nullptr;
static PFN_vkGetPhysicalDeviceQueueFamilyProperties FvkGetPhysicalDeviceQueueFamilyProperties = nullptr;
static PFN_vkGetPhysicalDeviceQueueFamilyProperties2 FvkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
static PFN_vkGetPhysicalDeviceFeatures FvkGetPhysicalDeviceFeatures = nullptr;
static PFN_vkGetPhysicalDeviceFeatures2 FvkGetPhysicalDeviceFeatures2 = nullptr;
static PFN_vkGetPhysicalDeviceMemoryProperties FvkGetPhysicalDeviceMemoryProperties = nullptr;
static PFN_vkGetPhysicalDeviceMemoryProperties2 FvkGetPhysicalDeviceMemoryProperties2 = nullptr;
static PFN_vkCreateDevice FvkCreateDevice = nullptr;
static PFN_vkDestroyDevice FvkDestroyDevice = nullptr;
static PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR FvkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
static PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR FvkGetPhysicalDeviceSurfaceCapabilities2KHR = nullptr;
static PFN_vkCreateWin32SurfaceKHR FvkCreateWin32SurfaceKHR = nullptr;
static PFN_vkDestroySurfaceKHR FvkDestroySurfaceKHR = nullptr;
static PFN_vkCreateSwapchainKHR FvkCreateSwapchainKHR = nullptr;
static PFN_vkDestroySwapchainKHR FvkDestroySwapchainKHR = nullptr;
static PFN_vkGetSwapchainImagesKHR FvkGetSwapchainImagesKHR = nullptr;
static PFN_vkAcquireNextImageKHR FvkAcquireNextImageKHR = nullptr;
static PFN_vkAcquireNextImage2KHR FvkAcquireNextImage2KHR = nullptr;
static PFN_vkGetDeviceQueue FvkGetDeviceQueue = nullptr;
static PFN_vkGetDeviceQueue2 FvkGetDeviceQueue2 = nullptr;
static PFN_vkQueueSubmit FvkQueueSubmit = nullptr;
static PFN_vkQueueSubmit2 FvkQueueSubmit2 = nullptr;
static PFN_vkQueuePresentKHR FvkQueuePresentKHR = nullptr;
static PFN_vkCreateImage FvkCreateImage = nullptr;
static PFN_vkDestroyImage FvkDestroyImage = nullptr;
static PFN_vkGetImageMemoryRequirements FvkGetImageMemoryRequirements = nullptr;
static PFN_vkGetImageMemoryRequirements2 FvkGetImageMemoryRequirements2 = nullptr;
static PFN_vkCreateImageView FvkCreateImageView = nullptr;
static PFN_vkDestroyImageView FvkDestroyImageView = nullptr;
static PFN_vkCreateCommandPool FvkCreateCommandPool = nullptr;
static PFN_vkDestroyCommandPool FvkDestroyCommandPool = nullptr;
static PFN_vkAllocateCommandBuffers FvkAllocateCommandBuffers = nullptr;
static PFN_vkFreeCommandBuffers FvkFreeCommandBuffers = nullptr;
static PFN_vkBeginCommandBuffer FvkBeginCommandBuffer = nullptr;
static PFN_vkEndCommandBuffer FvkEndCommandBuffer = nullptr;
static PFN_vkResetCommandBuffer FvkResetCommandBuffer = nullptr;
static PFN_vkDeviceWaitIdle FvkDeviceWaitIdle = nullptr;
static PFN_vkCreateFence FvkCreateFence = nullptr;
static PFN_vkDestroyFence FvkDestroyFence = nullptr;
static PFN_vkWaitForFences FvkWaitForFences = nullptr;
static PFN_vkResetFences FvkResetFences = nullptr;
static PFN_vkCreateSemaphore FvkCreateSemaphore = nullptr;
static PFN_vkDestroySemaphore FvkDestroySemaphore = nullptr;
static PFN_vkCreateDescriptorSetLayout FvkCreateDescriptorSetLayout = nullptr;
static PFN_vkDestroyDescriptorSetLayout FvkDestroyDescriptorSetLayout = nullptr;
static PFN_vkCreateDescriptorPool FvkCreateDescriptorPool = nullptr;
static PFN_vkDestroyDescriptorPool FvkDestroyDescriptorPool = nullptr;
static PFN_vkResetDescriptorPool FvkResetDescriptorPool = nullptr;
static PFN_vkAllocateDescriptorSets FvkAllocateDescriptorSets = nullptr;
static PFN_vkFreeDescriptorSets FvkFreeDescriptorSets = nullptr;
static PFN_vkUpdateDescriptorSets FvkUpdateDescriptorSets = nullptr;
static PFN_vkCreateGraphicsPipelines FvkCreateGraphicsPipelines = nullptr;
static PFN_vkDestroyPipeline FvkDestroyPipeline = nullptr;
static PFN_vkCreatePipelineLayout FvkCreatePipelineLayout = nullptr;
static PFN_vkDestroyPipelineLayout FvkDestroyPipelineLayout = nullptr;
static PFN_vkCreateShaderModule FvkCreateShaderModule = nullptr;
static PFN_vkDestroyShaderModule FvkDestroyShaderModule = nullptr;
static PFN_vkAllocateMemory FvkAllocateMemory = nullptr;
static PFN_vkFreeMemory FvkFreeMemory = nullptr;
static PFN_vkBindImageMemory FvkBindImageMemory = nullptr;
static PFN_vkBindImageMemory2 FvkBindImageMemory2 = nullptr;
static PFN_vkBindBufferMemory FvkBindBufferMemory = nullptr;
static PFN_vkBindBufferMemory2 FvkBindBufferMemory2 = nullptr;
static PFN_vkMapMemory FvkMapMemory = nullptr;
static PFN_vkUnmapMemory FvkUnmapMemory = nullptr;
static PFN_vkCreateBuffer FvkCreateBuffer = nullptr;
static PFN_vkDestroyBuffer FvkDestroyBuffer = nullptr;
static PFN_vkGetBufferMemoryRequirements FvkGetBufferMemoryRequirements = nullptr;
static PFN_vkGetBufferMemoryRequirements2 FvkGetBufferMemoryRequirements2 = nullptr;
static PFN_vkGetBufferDeviceAddress FvkGetBufferDeviceAddress = nullptr;
static PFN_vkCreateSampler FvkCreateSampler = nullptr;
static PFN_vkDestroySampler FvkDestroySampler = nullptr;
static PFN_vkCmdPipelineBarrier FvkCmdPipelineBarrier = nullptr;
static PFN_vkCmdPipelineBarrier2 FvkCmdPipelineBarrier2 = nullptr;
static PFN_vkCmdClearColorImage FvkCmdClearColorImage = nullptr;
static PFN_vkCmdClearDepthStencilImage FvkCmdClearDepthStencilImage = nullptr;
static PFN_vkCmdSetViewport FvkCmdSetViewport = nullptr;
static PFN_vkCmdSetScissor FvkCmdSetScissor = nullptr;
static PFN_vkCmdCopyBuffer FvkCmdCopyBuffer = nullptr;
static PFN_vkCmdCopyBuffer2 FvkCmdCopyBuffer2 = nullptr;
static PFN_vkCmdCopyBufferToImage FvkCmdCopyBufferToImage = nullptr;
static PFN_vkCmdCopyBufferToImage2 FvkCmdCopyBufferToImage2 = nullptr;
static PFN_vkCmdBlitImage FvkCmdBlitImage = nullptr;
static PFN_vkCmdBlitImage2 FvkCmdBlitImage2 = nullptr;
static PFN_vkCmdBindPipeline FvkCmdBindPipeline = nullptr;
static PFN_vkCmdPushConstants FvkCmdPushConstants = nullptr;
static PFN_vkCmdBindIndexBuffer FvkCmdBindIndexBuffer = nullptr;
static PFN_vkCmdBindDescriptorSets FvkCmdBindDescriptorSets = nullptr;
static PFN_vkCmdBeginRendering FvkCmdBeginRendering = nullptr;
static PFN_vkCmdEndRendering FvkCmdEndRendering = nullptr;
static PFN_vkCmdDraw FvkCmdDraw = nullptr;
static PFN_vkCmdDrawIndexed FvkCmdDrawIndexed = nullptr;

#define GET_INSTANCE(inst, fun) F##fun = reinterpret_cast<PFN_##fun>(window->getVulkanInstanceFunction(inst, #fun))
#define GET_INSTANCE_KHR(fun) F##fun = reinterpret_cast<PFN_##fun>(window->getVulkanInstanceFunctionKHR(#fun))
#define GET_DEVICE(dev, fun) F##fun = reinterpret_cast<PFN_##fun>(vkGetDeviceProcAddr(dev, #fun))

void Vulkan::loadStatic(const std::shared_ptr<Win32Window>& window) {
    GET_INSTANCE(nullptr, vkCreateInstance);
}

void Vulkan::loadInstance(const std::shared_ptr<Win32Window>& window, VkInstance instance) {
    GET_INSTANCE(instance, vkDestroyInstance);
    GET_INSTANCE(instance, vkCreateDebugUtilsMessengerEXT);
    GET_INSTANCE(instance, vkDestroyDebugUtilsMessengerEXT);
    GET_INSTANCE(instance, vkGetDeviceProcAddr);
    GET_INSTANCE(instance, vkEnumeratePhysicalDevices);
    GET_INSTANCE(instance, vkGetPhysicalDeviceProperties);
    GET_INSTANCE(instance, vkGetPhysicalDeviceProperties2);
    GET_INSTANCE(instance, vkEnumerateDeviceExtensionProperties);
    GET_INSTANCE(instance, vkGetPhysicalDeviceQueueFamilyProperties);
    GET_INSTANCE(instance, vkGetPhysicalDeviceQueueFamilyProperties2);
    GET_INSTANCE(instance, vkGetPhysicalDeviceFeatures);
    GET_INSTANCE(instance, vkGetPhysicalDeviceFeatures2);
    GET_INSTANCE(instance, vkGetPhysicalDeviceMemoryProperties);
    GET_INSTANCE(instance, vkGetPhysicalDeviceMemoryProperties2);
    GET_INSTANCE(instance, vkCreateDevice);
    GET_INSTANCE(instance, vkDestroyDevice);
    GET_INSTANCE_KHR(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    GET_INSTANCE_KHR(vkGetPhysicalDeviceSurfaceCapabilities2KHR);
    GET_INSTANCE_KHR(vkCreateWin32SurfaceKHR);
    GET_INSTANCE_KHR(vkDestroySurfaceKHR);
}

void Vulkan::loadDevice(VkDevice device) {
    GET_DEVICE(device, vkCreateSwapchainKHR);
    GET_DEVICE(device, vkDestroySwapchainKHR);
    GET_DEVICE(device, vkGetSwapchainImagesKHR);
    GET_DEVICE(device, vkAcquireNextImageKHR);
    GET_DEVICE(device, vkAcquireNextImage2KHR);
    GET_DEVICE(device, vkGetDeviceQueue);
    GET_DEVICE(device, vkGetDeviceQueue2);
    GET_DEVICE(device, vkQueueSubmit);
    GET_DEVICE(device, vkQueueSubmit2);
    GET_DEVICE(device, vkQueuePresentKHR);
    GET_DEVICE(device, vkCreateImage);
    GET_DEVICE(device, vkDestroyImage);
    GET_DEVICE(device, vkGetImageMemoryRequirements);
    GET_DEVICE(device, vkGetImageMemoryRequirements2);
    GET_DEVICE(device, vkCreateImageView);
    GET_DEVICE(device, vkDestroyImageView);
    GET_DEVICE(device, vkCreateCommandPool);
    GET_DEVICE(device, vkDestroyCommandPool);
    GET_DEVICE(device, vkAllocateCommandBuffers);
    GET_DEVICE(device, vkFreeCommandBuffers);
    GET_DEVICE(device, vkBeginCommandBuffer);
    GET_DEVICE(device, vkEndCommandBuffer);
    GET_DEVICE(device, vkResetCommandBuffer);
    GET_DEVICE(device, vkDeviceWaitIdle);
    GET_DEVICE(device, vkCreateFence);
    GET_DEVICE(device, vkDestroyFence);
    GET_DEVICE(device, vkWaitForFences);
    GET_DEVICE(device, vkResetFences);
    GET_DEVICE(device, vkCreateSemaphore);
    GET_DEVICE(device, vkDestroySemaphore);
    GET_DEVICE(device, vkCreateDescriptorSetLayout);
    GET_DEVICE(device, vkDestroyDescriptorSetLayout);
    GET_DEVICE(device, vkCreateDescriptorPool);
    GET_DEVICE(device, vkDestroyDescriptorPool);
    GET_DEVICE(device, vkResetDescriptorPool);
    GET_DEVICE(device, vkAllocateDescriptorSets);
    GET_DEVICE(device, vkFreeDescriptorSets);
    GET_DEVICE(device, vkUpdateDescriptorSets);
    GET_DEVICE(device, vkCreateGraphicsPipelines);
    GET_DEVICE(device, vkDestroyPipeline);
    GET_DEVICE(device, vkCreatePipelineLayout);
    GET_DEVICE(device, vkDestroyPipelineLayout);
    GET_DEVICE(device, vkCreateShaderModule);
    GET_DEVICE(device, vkDestroyShaderModule);
    GET_DEVICE(device, vkAllocateMemory);
    GET_DEVICE(device, vkFreeMemory);
    GET_DEVICE(device, vkBindImageMemory);
    GET_DEVICE(device, vkBindImageMemory2);
    GET_DEVICE(device, vkBindBufferMemory);
    GET_DEVICE(device, vkBindBufferMemory2);
    GET_DEVICE(device, vkMapMemory);
    GET_DEVICE(device, vkUnmapMemory);
    GET_DEVICE(device, vkCreateBuffer);
    GET_DEVICE(device, vkDestroyBuffer);
    GET_DEVICE(device, vkGetBufferMemoryRequirements);
    GET_DEVICE(device, vkGetBufferMemoryRequirements2);
    GET_DEVICE(device, vkGetBufferDeviceAddress);
    GET_DEVICE(device, vkCreateSampler);
    GET_DEVICE(device, vkDestroySampler);
    GET_DEVICE(device, vkCmdPipelineBarrier);
    GET_DEVICE(device, vkCmdPipelineBarrier2);
    GET_DEVICE(device, vkCmdClearColorImage);
    GET_DEVICE(device, vkCmdClearDepthStencilImage);
    GET_DEVICE(device, vkCmdSetViewport);
    GET_DEVICE(device, vkCmdSetScissor);
    GET_DEVICE(device, vkCmdCopyBuffer);
    GET_DEVICE(device, vkCmdCopyBuffer2);
    GET_DEVICE(device, vkCmdCopyBufferToImage);
    GET_DEVICE(device, vkCmdCopyBufferToImage2);
    GET_DEVICE(device, vkCmdBlitImage);
    GET_DEVICE(device, vkCmdBlitImage2);
    GET_DEVICE(device, vkCmdBindPipeline);
    GET_DEVICE(device, vkCmdPushConstants);
    GET_DEVICE(device, vkCmdBindIndexBuffer);
    GET_DEVICE(device, vkCmdBindDescriptorSets);
    GET_DEVICE(device, vkCmdBeginRendering);
    GET_DEVICE(device, vkCmdEndRendering);
    GET_DEVICE(device, vkCmdDraw);
    GET_DEVICE(device, vkCmdDrawIndexed);
}

VkResult Vulkan::vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    return FvkCreateInstance(pCreateInfo, pAllocator, pInstance);
}

void Vulkan::vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyInstance(instance, pAllocator);
}

VkResult Vulkan::vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger) {
    return FvkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

void Vulkan::vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

PFN_vkVoidFunction Vulkan::vkGetDeviceProcAddr(VkDevice device, const char* pName) {
    return FvkGetDeviceProcAddr(device, pName);
}

VkResult Vulkan::vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) {
    return FvkEnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
}

void Vulkan::vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties) {
    FvkGetPhysicalDeviceProperties(physicalDevice, pProperties);
}

void Vulkan::vkGetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties) {
    FvkGetPhysicalDeviceProperties2(physicalDevice, pProperties);
}

VkResult Vulkan::vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
    return FvkEnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount, pProperties);
}

void Vulkan::vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties) {
    FvkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

void Vulkan::vkGetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties) {
    FvkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

void Vulkan::vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures) {
    FvkGetPhysicalDeviceFeatures(physicalDevice, pFeatures);
}

void Vulkan::vkGetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures) {
    FvkGetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
}

void Vulkan::vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties) {
    FvkGetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
}

void Vulkan::vkGetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties) {
    FvkGetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
}

VkResult Vulkan::vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    return FvkCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);
}

void Vulkan::vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyDevice(device, pAllocator);
}

VkResult Vulkan::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) {
    return FvkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
}

VkResult Vulkan::vkGetPhysicalDeviceSurfaceCapabilities2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo, VkSurfaceCapabilities2KHR* pSurfaceCapabilities) {
    return FvkGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, pSurfaceInfo, pSurfaceCapabilities);
}

VkResult Vulkan::vkCreateWin32SurfaceKHR(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    return FvkCreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
}

void Vulkan::vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator) {
    FvkDestroySurfaceKHR(instance, surface, pAllocator);
}

VkResult Vulkan::vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) {
    return FvkCreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
}

void Vulkan::vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator) {
    FvkDestroySwapchainKHR(device, swapchain, pAllocator);
}

VkResult Vulkan::vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages) {
    return FvkGetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);
}

VkResult Vulkan::vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) {
    return FvkAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
}

VkResult Vulkan::vkAcquireNextImageKHR2(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex) {
    return FvkAcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
}

void Vulkan::vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue) {
    FvkGetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
}

void Vulkan::vkGetDeviceQueue2(VkDevice device, const VkDeviceQueueInfo2* pQueueInfo, VkQueue* pQueue) {
    FvkGetDeviceQueue2(device, pQueueInfo, pQueue);
}

VkResult Vulkan::vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
    return FvkQueueSubmit(queue, submitCount, pSubmits, fence);
}

VkResult Vulkan::vkQueueSubmit2(VkQueue queue, uint32_t submitCount, const VkSubmitInfo2* pSubmits, VkFence fence) {
    return FvkQueueSubmit2(queue, submitCount, pSubmits, fence);
}

VkResult Vulkan::vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) {
    return FvkQueuePresentKHR(queue, pPresentInfo);
}

VkResult Vulkan::vkCreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage) {
    return FvkCreateImage(device, pCreateInfo, pAllocator, pImage);
}

void Vulkan::vkDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyImage(device, image, pAllocator);
}

void Vulkan::vkGetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements) {
    FvkGetImageMemoryRequirements(device, image, pMemoryRequirements);
}

void Vulkan::vkGetImageMemoryRequirements2(VkDevice device, const VkImageMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements) {
    FvkGetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

VkResult Vulkan::vkCreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView) {
    return FvkCreateImageView(device, pCreateInfo, pAllocator, pView);
}

void Vulkan::vkDestroyImageView(VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyImageView(device, imageView, pAllocator);
}

VkResult Vulkan::vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) {
    return FvkCreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
}

void Vulkan::vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyCommandPool(device, commandPool, pAllocator);
}

VkResult Vulkan::vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) {
    return FvkAllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
}

void Vulkan::vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
    FvkFreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
}

VkResult Vulkan::vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) {
    return FvkBeginCommandBuffer(commandBuffer, pBeginInfo);
}

VkResult Vulkan::vkEndCommandBuffer(VkCommandBuffer commandBuffer) {
    return FvkEndCommandBuffer(commandBuffer);
}

VkResult Vulkan::vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) {
    return FvkResetCommandBuffer(commandBuffer, flags);
}

VkResult Vulkan::vkDeviceWaitIdle(VkDevice device) {
    return FvkDeviceWaitIdle(device);
}

VkResult Vulkan::vkCreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) {
    return FvkCreateFence(device, pCreateInfo, pAllocator, pFence);
}

void Vulkan::vkDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyFence(device, fence, pAllocator);
}

VkResult Vulkan::vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout) {
    return FvkWaitForFences(device, fenceCount, pFences, waitAll, timeout);
}

VkResult Vulkan::vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences) {
    return FvkResetFences(device, fenceCount, pFences);
}

VkResult Vulkan::vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) {
    return FvkCreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
}

void Vulkan::vkDestroySemaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator) {
    FvkDestroySemaphore(device, semaphore, pAllocator);
}

VkResult Vulkan::vkCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout) {
    return FvkCreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
}

void Vulkan::vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
}

VkResult Vulkan::vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool) {
    return FvkCreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
}

void Vulkan::vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyDescriptorPool(device, descriptorPool, pAllocator);
}

VkResult Vulkan::vkResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags) {
    return FvkResetDescriptorPool(device, descriptorPool, flags);
}

VkResult Vulkan::vkAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets) {
    return FvkAllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
}

VkResult Vulkan::vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets) {
    return FvkFreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
}

void Vulkan::vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies) {
    FvkUpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}

VkResult Vulkan::vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) {
    return FvkCreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

void Vulkan::vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyPipeline(device, pipeline, pAllocator);
}

VkResult Vulkan::vkCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout) {
    return FvkCreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
}

void Vulkan::vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyPipelineLayout(device, pipelineLayout, pAllocator);
}

VkResult Vulkan::vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule) {
    return FvkCreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
}

void Vulkan::vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyShaderModule(device, shaderModule, pAllocator);
}

VkResult Vulkan::vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory) {
    return FvkAllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
}

void Vulkan::vkFreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator) {
    FvkFreeMemory(device, memory, pAllocator);
}

VkResult Vulkan::vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset) {
    return FvkBindImageMemory(device, image, memory, memoryOffset);
}

VkResult Vulkan::vkBindImageMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos) {
    return FvkBindImageMemory2(device, bindInfoCount, pBindInfos);
}

VkResult Vulkan::vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) {
    return FvkBindBufferMemory(device, buffer, memory, memoryOffset);
}

VkResult Vulkan::vkBindBufferMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos) {
    return FvkBindBufferMemory2(device, bindInfoCount, pBindInfos);
}

VkResult Vulkan::vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData) {
    return FvkMapMemory(device, memory, offset, size, flags, ppData);
}

void Vulkan::vkUnmapMemory(VkDevice device, VkDeviceMemory memory) {
    FvkUnmapMemory(device, memory);
}

VkResult Vulkan::vkCreateBuffer(VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer) {
    return FvkCreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
}

void Vulkan::vkDestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator) {
    FvkDestroyBuffer(device, buffer, pAllocator);
}

void Vulkan::vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements) {
    FvkGetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
}

void Vulkan::vkGetBufferMemoryRequirements2(VkDevice device, const VkBufferMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements) {
    FvkGetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

VkDeviceAddress Vulkan::vkGetBufferDeviceAddress(VkDevice device, const VkBufferDeviceAddressInfo* pInfo) {
    return FvkGetBufferDeviceAddress(device, pInfo);
}

VkResult Vulkan::vkCreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) {
    return FvkCreateSampler(device, pCreateInfo, pAllocator, pSampler);
}

void Vulkan::vkDestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator) {
    FvkDestroySampler(device, sampler, pAllocator);
}

void Vulkan::vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) {
    FvkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

void Vulkan::vkCmdPipelineBarrier2(VkCommandBuffer commandBuffer, const VkDependencyInfo* pDependencyInfo) {
    FvkCmdPipelineBarrier2(commandBuffer, pDependencyInfo);
}

void Vulkan::vkCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges) {
    FvkCmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
}

void Vulkan::vkCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges) {
    FvkCmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
}

void Vulkan::vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports) {
    FvkCmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
}

void Vulkan::vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors) {
    FvkCmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
}

void Vulkan::vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions) {
    FvkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
}

void Vulkan::vkCmdCopyBuffer2(VkCommandBuffer commandBuffer, const VkCopyBufferInfo2* pCopyBufferInfo) {
    FvkCmdCopyBuffer2(commandBuffer, pCopyBufferInfo);
}

void Vulkan::vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions) {
    FvkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
}

void Vulkan::vkCmdCopyBufferToImage2(VkCommandBuffer commandBuffer, const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo) {
    FvkCmdCopyBufferToImage2(commandBuffer, pCopyBufferToImageInfo);
}

void Vulkan::vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter) {
    FvkCmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
}

void Vulkan::vkCmdBlitImage2(VkCommandBuffer commandBuffer, const VkBlitImageInfo2* pBlitImageInfo) {
    FvkCmdBlitImage2(commandBuffer, pBlitImageInfo);
}

void Vulkan::vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) {
    FvkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}

void Vulkan::vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues) {
    FvkCmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
}

void Vulkan::vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) {
    FvkCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
}

void Vulkan::vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) {
    FvkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
}

void Vulkan::vkCmdBeginRendering(VkCommandBuffer commandBuffer, const VkRenderingInfo* pRenderingInfo) {
    FvkCmdBeginRendering(commandBuffer, pRenderingInfo);
}

void Vulkan::vkCmdEndRendering(VkCommandBuffer commandBuffer) {
    FvkCmdEndRendering(commandBuffer);
}

void Vulkan::vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    FvkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void Vulkan::vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    FvkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
