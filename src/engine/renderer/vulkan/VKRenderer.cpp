#include "VKRenderer.h"

#include "Vulkan.h"
#include "../../../utils/Logger.h"
#include "descriptor/DescriptorLayoutBuilder.h"
#include "descriptor/DescriptorWriter.h"
#include "pipeline/PipelineUtils.h"
#include "tessellation/Mesh.h"
#include "utils/BufferUtils.h"
#include "utils/ImageUtils.h"
#include "utils/Init.h"
#include "utils/Vertex.h"

bool VKRenderer::init() {
    this->tessellator = std::make_shared<Tessellator>(this);

    Vulkan::loadStatic(this->window);

    this->initInstance();

    Vulkan::loadInstance(this->window, this->instance);

    this->initMessenger();

    if (!this->initDevice())
        return false;

    Vulkan::loadDevice(this->device);

    this->surface = this->window->createVulkanSurface(this->instance, nullptr);

    VkDeviceQueueInfo2 queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
    queueInfo.pNext = nullptr;

    queueInfo.flags = 0;
    queueInfo.queueFamilyIndex = this->graphicsQueueFamily;
    queueInfo.queueIndex = 0;

    Vulkan::vkGetDeviceQueue2(this->device, &queueInfo, &this->graphicsQueue);

    const auto size = this->window->getWindowSize();

    this->initSwapchain(size.x, size.y);
    this->initMatrices();
    this->allocateBackBuffer();
    this->initDescriptors();
    this->initCommands();
    this->initSyncStructures();
    this->initPipelines();
    this->initTextures();

    this->context->init();

    return true;
}

void VKRenderer::exit() {
    if (this->instance == nullptr)
        return;

    if (this->device != nullptr) {
        VK_CHECK(vkDeviceWaitIdle, this->device);

        this->context->exit();

        this->destroyTextures();
        this->destroyMeshes();
        this->destroyPipelines();
        this->destroyFrameData();
        this->destroyDescriptors();
        this->freeBackBuffer();
        this->destroySwapchain();

        Vulkan::vkDestroySurfaceKHR(this->instance, this->surface, nullptr);

        this->destroyDevice();
    }

    this->destroyMessenger();
    this->destroyInstance();
}

void VKRenderer::render(const double deltaTime) {
    if (this->resizeRequested) {
        VK_CHECK(vkDeviceWaitIdle, this->device);

        const auto size = this->window->getWindowSize();

        this->recreateSwapchain(size.x, size.y);
        this->initMatrices();

        this->resizeRequested = false;
    }

    auto& frame = this->getCurrentFrame();

    constexpr uint64_t timeout = 1'000'000'000;

    VK_CHECK(vkWaitForFences, this->device, 1, &frame.fence, VK_TRUE, timeout);
    VK_CHECK(vkResetFences, this->device, 1, &frame.fence);

    frame.frameDescriptors.clearPools(this->device);

    uint32_t swapchainImageIndex = 0;

#ifdef _M_IX86
    const auto nextImage = Vulkan::vkAcquireNextImageKHR(this->device, this->swapchain, timeout, frame.swapchainSemaphore, NULL, &swapchainImageIndex);
#else
    const auto nextImage = Vulkan::vkAcquireNextImageKHR(this->device, this->swapchain, timeout, frame.swapchainSemaphore, nullptr, &swapchainImageIndex);
#endif

    if (nextImage == VK_ERROR_OUT_OF_DATE_KHR) {
        this->resizeRequested = true;
        return;
    }

    const auto cmd = frame.cmd;
    const auto img = this->swapchainImages[swapchainImageIndex];
    const auto backBuffer = this->backBuffer.image;
    const auto depthBuffer = this->depthImage.image;

    VK_CHECK(vkResetCommandBuffer, cmd, 0);

    const auto beginInfo = VK::Init::createCommandBufferBegin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer, cmd, &beginInfo);

    const VkViewport viewport{0.f, 0.f, static_cast<float>(this->swapchainExtent.width), static_cast<float>(this->swapchainExtent.height), 0.f, 1.f};
    Vulkan::vkCmdSetViewport(cmd, 0, 1, &viewport);

    const VkRect2D scissor{{0, 0}, this->swapchainExtent};
    Vulkan::vkCmdSetScissor(cmd, 0, 1, &scissor);

    {
        VK::ImageUtils::transitionImageToClear(cmd, backBuffer, VK_IMAGE_LAYOUT_UNDEFINED);

        constexpr VkClearColorValue clear = {{0.f, 0.f, 0.f, 0.f}};

        const auto clearRange = VK::Init::createImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

        Vulkan::vkCmdClearColorImage(cmd, backBuffer, VK_IMAGE_LAYOUT_GENERAL, &clear, 1, &clearRange);
    }

    VK::ImageUtils::transitionImageToRendering(cmd, backBuffer, VK_IMAGE_LAYOUT_GENERAL);
    VK::ImageUtils::transitionImage(cmd, depthBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_NONE, VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT);

    const auto colorAttachment = VK::Init::createRenderingAttachmentInfo(this->backBuffer.imageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    const auto depthAttachment = VK::Init::createDepthAttachmentInfo(this->depthImage.imageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
    const auto renderInfo = VK::Init::createRenderingInfo(this->swapchainExtent, &colorAttachment, &depthAttachment);
    Vulkan::vkCmdBeginRendering(cmd, &renderInfo);

    {
        auto& defMatrix = this->stack.push();

        defMatrix = translate(defMatrix, {0.f, 0.f, -250.f});

        this->context->render(deltaTime);

        this->stack.pop();

        this->stack.reset();
    }

    Vulkan::vkCmdEndRendering(cmd);

    VK::ImageUtils::transitionImageToCopying(cmd, backBuffer, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    VK::ImageUtils::transitionImageToCopying(cmd, img, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VK::ImageUtils::copyImageToImage(cmd, backBuffer, img, this->swapchainExtent, this->swapchainExtent);

    VK::ImageUtils::transitionImageToPresent(cmd, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VK_CHECK(vkEndCommandBuffer, cmd);

    const auto cmdInfo = VK::Init::createCommandBufferSubmit(cmd);

    const auto waitInfo = VK::Init::createSemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, frame.swapchainSemaphore);
    const auto signalInfo = VK::Init::createSemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, frame.renderSemaphore);

    const auto submitInfo = VK::Init::createSubmitInfo(&cmdInfo, &signalInfo, &waitInfo);

    VK_CHECK(vkQueueSubmit2, this->graphicsQueue, 1, &submitInfo, frame.fence);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &frame.renderSemaphore;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &this->swapchain;

    presentInfo.pImageIndices = &swapchainImageIndex;

    presentInfo.pResults = nullptr;

    const auto queue = Vulkan::vkQueuePresentKHR(this->graphicsQueue, &presentInfo);

    if (queue == VK_ERROR_OUT_OF_DATE_KHR)
        this->resizeRequested = true;

    this->frameNumber++;

    if (this->frameNumber >= BUFFER_COUNT)
        this->frameNumber = 0;
}

Renderer::Type VKRenderer::getRendererType() {
    return Type::Vulkan;
}

uint32_t VKRenderer::loadShader(const void* vertexShaderData, const size_t vertexShaderSize, const void* fragmentShaderData, const size_t fragmentShaderSize, std::span<std::string> attributes) {
    VK::Pipeline pipeline{};

    if (std::ranges::find(attributes.begin(), attributes.end(), "UV") != attributes.end())
        VK::PipelineUtils::createSamplerPipeline(this->device, nullptr, this->backBuffer.imageFormat, vertexShaderData, vertexShaderSize, fragmentShaderData, fragmentShaderSize, pipeline, this->singleImageDescriptorLayout);
    else
        VK::PipelineUtils::createPipeline(this->device, nullptr, this->backBuffer.imageFormat, vertexShaderData, vertexShaderSize, fragmentShaderData, fragmentShaderSize, pipeline);

    const auto id = this->shaders.size();

    this->shaders.emplace_back(pipeline);

    return static_cast<uint32_t>(id);
}

std::shared_ptr<Mesh> VKRenderer::upload(float* vertices, const uint32_t vertexCount, uint16_t* indices, const uint32_t indexCount, const uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes) {
    auto mesh = std::make_shared<VK::Mesh>();

    mesh->load(vertices, vertexCount, indices, indexCount, vertexSize, enabledAttributes, this);

    uint8_t index = 0;

    if (enabledAttributes[VertexFormats::VertexAttribute::ATTR_COLOR])
        index |= 0b1;

    if (enabledAttributes[VertexFormats::VertexAttribute::ATTR_TEXCOORD])
        index |= 0b10;

    mesh->setShader(this->defaultShaders[index]);

    this->meshes.emplace_back(mesh);

    return mesh;
}

uint32_t VKRenderer::loadTexture(const void* textureData, const size_t textureSize, const uint32_t width, const uint32_t height) {
    const auto image = this->createImage(textureData, {width, height, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

    const auto id = this->textures.size();

    this->textures.emplace_back(image);

    return static_cast<uint32_t>(id);
}

void VKRenderer::draw(const std::shared_ptr<Mesh> mesh) {
    if (mesh->getShaderId() >= this->shaders.size())
        return;

    const auto cmd = this->getCurrentFrame().cmd;
    auto& pipeline = this->shaders[mesh->getShaderId()];

    Vulkan::vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);

    DrawData data{};

    data.cmd = cmd;
    data.pipeline = &pipeline;
    data.matrix = this->projectionMatrix * this->viewMatrix * this->stack.top();
    data.shaderColor = this->shaderColor;

    mesh->draw(&data);
}

void VKRenderer::draw(const std::shared_ptr<Mesh> mesh, const uint32_t texId) {
    if (mesh->getShaderId() >= this->shaders.size())
        return;

    const auto image = texId < this->textures.size() ? this->textures[texId].imageView : this->missingTexture.second->imageView;

    const auto cmd = this->getCurrentFrame().cmd;
    auto& pipeline = this->shaders[mesh->getShaderId()];

    Vulkan::vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);

    {
        const auto set = this->getCurrentFrame().frameDescriptors.allocate(this->device, this->singleImageDescriptorLayout);

        VK::DescriptorWriter writer{};
        writer.writeImage(0, image, this->nearestSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        writer.updateSet(this->device, set);

        Vulkan::vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.layout, 0, 1, &set, 0, nullptr);
    }

    DrawData data{};

    data.cmd = cmd;
    data.pipeline = &pipeline;
    data.matrix = this->projectionMatrix * this->viewMatrix * this->stack.top();
    data.shaderColor = this->shaderColor;

    mesh->draw(&data);
}

void VKRenderer::uploadMeshRaw(const std::span<VK::Vertex> vertices, const uint16_t* indices, const uint32_t indexCount, VK::Mesh& mesh) const {
    const size_t vertexBufferSize = vertices.size() * sizeof(VK::Vertex);
    const size_t indexBufferSize = indexCount * sizeof(uint16_t);

    VK::BufferUtils::createBuffer(this->device, nullptr, vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        this->memoryProperties, mesh.vertexBuffer);

    VkBufferDeviceAddressInfo addressInfo{};
    addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    addressInfo.pNext = nullptr;
    addressInfo.buffer = mesh.vertexBuffer.buffer;

    mesh.vertexBufferAddress = Vulkan::vkGetBufferDeviceAddress(this->device, &addressInfo);

    VK::BufferUtils::createBuffer(this->device, nullptr, indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->memoryProperties, mesh.indexBuffer);

    VK::Buffer staging{};
    VK::BufferUtils::createBuffer(this->device, nullptr, vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->memoryProperties, staging);

    void* data = nullptr;
    VK_CHECK(vkMapMemory, this->device, staging.memory, 0, vertexBufferSize + indexBufferSize, 0, &data);

    memcpy(data, vertices.data(), vertexBufferSize);
    memcpy(static_cast<char*>(data) + vertexBufferSize, indices, indexBufferSize);

    this->executeImm([&](VkCommandBuffer cmd) {
        {
            VkBufferCopy2 copy{};
            copy.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
            copy.pNext = nullptr;

            copy.srcOffset = 0;
            copy.dstOffset = 0;
            copy.size = vertexBufferSize;

            VkCopyBufferInfo2 info{};
            info.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
            info.pNext = nullptr;

            info.srcBuffer = staging.buffer;
            info.dstBuffer = mesh.vertexBuffer.buffer;

            info.regionCount = 1;
            info.pRegions = &copy;

            Vulkan::vkCmdCopyBuffer2(cmd, &info);
        }

        {
            VkBufferCopy2 copy{};
            copy.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
            copy.pNext = nullptr;

            copy.srcOffset = vertexBufferSize;
            copy.dstOffset = 0;
            copy.size = indexBufferSize;

            VkCopyBufferInfo2 info{};
            info.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
            info.pNext = nullptr;

            info.srcBuffer = staging.buffer;
            info.dstBuffer = mesh.indexBuffer.buffer;

            info.regionCount = 1;
            info.pRegions = &copy;

            Vulkan::vkCmdCopyBuffer2(cmd, &info);
        }
    });

    Vulkan::vkUnmapMemory(this->device, staging.memory);

    VK::BufferUtils::destroyBuffer(this->device, nullptr, staging);
}

void VKRenderer::initInstance() {
    std::vector<const char*> layers{};

#ifndef NDEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    std::vector<const char*> extensions = this->window->getRequiredExtensions();

#ifndef NDEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;

    appInfo.pApplicationName = "BorionLauncher";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "BorionVK";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_MAKE_VERSION(1, 3, 0);

    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext = nullptr;

    info.flags = 0;
    info.pApplicationInfo = &appInfo;
    info.enabledExtensionCount = extensions.size();
    info.ppEnabledExtensionNames = extensions.data();
    info.enabledLayerCount = layers.size();
    info.ppEnabledLayerNames = layers.data();

    VK_CHECK(vkCreateInstance, &info, nullptr, &this->instance);
}

void VKRenderer::destroyInstance() const {
    Vulkan::vkDestroyInstance(this->instance, nullptr);
}

void VKRenderer::initMessenger() {
#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.pNext = nullptr;

    createInfo.flags = 0;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugMessengerCallback;
    createInfo.pUserData = nullptr;

    VK_CHECK(vkCreateDebugUtilsMessengerEXT, this->instance, &createInfo, nullptr, &this->debugMessenger);
#endif
}

void VKRenderer::destroyMessenger() const {
#ifndef NDEBUG
    Vulkan::vkDestroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, nullptr);
#endif
}

bool VKRenderer::initDevice() {
    this->choosePhysicalDevice();

    if (this->physicalDevice == VK_NULL_HANDLE) {
        logF("[ERROR] Couldn't find suitable physical device");
        return false;
    }

    std::vector<const char*> extensions{};
    extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    std::vector<VkDeviceQueueCreateInfo> queueInfos{};
    std::vector<std::vector<float>> queuePrios{};

    {
        uint32_t queueFamilyCount = 0;
        Vulkan::vkGetPhysicalDeviceQueueFamilyProperties2(this->physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties2> queueFamilyProperties{queueFamilyCount};

        for (auto& property : queueFamilyProperties) {
            property.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        }

        Vulkan::vkGetPhysicalDeviceQueueFamilyProperties2(this->physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        queuePrios.resize(queueFamilyCount);

        for (int i = 0; i < queueFamilyCount; i++) {
            const auto& property = queueFamilyProperties[i];
            auto& prios = queuePrios[i];

            prios.resize(property.queueFamilyProperties.queueCount);

            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.pNext = nullptr;

            queueInfo.flags = 0;
            queueInfo.queueFamilyIndex = i;
            queueInfo.queueCount = property.queueFamilyProperties.queueCount;
            queueInfo.pQueuePriorities = prios.data();

            queueInfos.emplace_back(queueInfo);
        }
    }

    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.pNext = nullptr;
    features13.dynamicRendering = true;
    features13.synchronization2 = true;

    VkPhysicalDeviceVulkan12Features features12{};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.pNext = &features13;
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;

    VkPhysicalDeviceFeatures2 features2{};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &features12;

    VkDeviceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = &features2;

    info.flags = 0;
    info.queueCreateInfoCount = queueInfos.size();
    info.pQueueCreateInfos = queueInfos.data();
    info.enabledLayerCount = 0;
    info.ppEnabledLayerNames = nullptr;
    info.enabledExtensionCount = extensions.size();
    info.ppEnabledExtensionNames = extensions.data();

    VK_CHECK(vkCreateDevice, this->physicalDevice, &info, nullptr, &this->device);
    return true;
}

void VKRenderer::destroyDevice() const {
    Vulkan::vkDestroyDevice(this->device, nullptr);
}

void VKRenderer::initSwapchain(const uint32_t width, const uint32_t height) {
    this->swapchainExtent = {width, height};

    VkSurfaceCapabilitiesKHR capabilities{};
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, this->physicalDevice, this->surface, &capabilities);

    this->swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

    VkSwapchainCreateInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.pNext = nullptr;

    info.flags = 0;
    info.surface = this->surface;
    info.minImageCount = BUFFER_COUNT;
    info.imageFormat = this->swapchainImageFormat;
    info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    info.imageExtent = this->swapchainExtent;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    info.preTransform = capabilities.currentTransform;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    info.presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
    info.clipped = VK_TRUE;

    VK_CHECK(vkCreateSwapchainKHR, this->device, &info, nullptr, &this->swapchain);

    uint32_t swapchainImageCount = 0;
    VK_CHECK_N(1, vkGetSwapchainImagesKHR, this->device, this->swapchain, &swapchainImageCount, nullptr);

    this->swapchainImages.resize(swapchainImageCount);
    this->swapchainImageViews.resize(swapchainImageCount);
    VK_CHECK_N(2, vkGetSwapchainImagesKHR, this->device, this->swapchain, &swapchainImageCount, this->swapchainImages.data());

    for (int i = 0; i < swapchainImageCount; i++) {
        const auto viewInfo = VK::Init::createImageView(this->swapchainImages[i], this->swapchainImageFormat);

        VK_CHECK(vkCreateImageView, this->device, &viewInfo, nullptr, &this->swapchainImageViews[i]);
    }
}

void VKRenderer::destroySwapchain() const {
    Vulkan::vkDestroySwapchainKHR(this->device, this->swapchain, nullptr);

    for (const auto view : this->swapchainImageViews) {
        Vulkan::vkDestroyImageView(this->device, view, nullptr);
    }
}

void VKRenderer::recreateSwapchain(const uint32_t width, const uint32_t height) {
    this->destroySwapchain();
    this->freeBackBuffer();
    this->initSwapchain(width, height);
    this->allocateBackBuffer();
}

void VKRenderer::allocateBackBuffer() {
    VkImageUsageFlags drawFlags{};
    drawFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const VkExtent3D imageExtent{
        this->swapchainExtent.width,
        this->swapchainExtent.height,
        1
    };

    this->backBuffer = this->createImage(imageExtent, VK_FORMAT_R16G16B16A16_SFLOAT, drawFlags);

    VkImageUsageFlags depthFlags{};
    depthFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    this->depthImage = this->createImage(imageExtent, VK_FORMAT_D32_SFLOAT, depthFlags);
}

void VKRenderer::freeBackBuffer() const {
    this->destroyImage(this->backBuffer);
    this->destroyImage(this->depthImage);
}

void VKRenderer::initDescriptors() {
    {
        VK::DescriptorLayoutBuilder builder{};
        builder.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        this->singleImageDescriptorLayout = builder.build(this->device, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr);
    }

    for (auto& frame : this->frames) {
        std::vector<VK::PoolSizeRatio> frameSizes{
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4}
        };

        frame.frameDescriptors.init(this->device, 1000, frameSizes);
    }
}

void VKRenderer::destroyDescriptors() const {
    Vulkan::vkDestroyDescriptorSetLayout(this->device, this->singleImageDescriptorLayout, nullptr);
}

void VKRenderer::initCommands() {
    const auto poolInfo = VK::Init::createCommandPool(this->graphicsQueueFamily);

    for (auto& frame : this->frames) {
        VK_CHECK(vkCreateCommandPool, this->device, &poolInfo, nullptr, &frame.pool);

        const auto cmdInfo = VK::Init::createCommandBuffer(frame.pool);

        VK_CHECK(vkAllocateCommandBuffers, this->device, &cmdInfo, &frame.cmd);
    }

    VK_CHECK(vkCreateCommandPool, this->device, &poolInfo, nullptr, &this->immCmdPool);

    const auto allocInfo = VK::Init::createCommandBuffer(this->immCmdPool);
    VK_CHECK(vkAllocateCommandBuffers, this->device, &allocInfo, &this->immCmdBuffer);
}

void VKRenderer::initSyncStructures() {
    const auto fenceInfo = VK::Init::createFenceInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    const auto semaphoreInfo = VK::Init::createSemaphoreInfo();

    for (auto& frame : this->frames) {
        VK_CHECK(vkCreateFence, this->device, &fenceInfo, nullptr, &frame.fence);

        VK_CHECK(vkCreateSemaphore, this->device, &semaphoreInfo, nullptr, &frame.swapchainSemaphore);
        VK_CHECK(vkCreateSemaphore, this->device, &semaphoreInfo, nullptr, &frame.renderSemaphore);
    }

    VK_CHECK(vkCreateFence, this->device, &fenceInfo, nullptr, &this->immFence);
}

void VKRenderer::destroyFrameData() {
    Vulkan::vkDestroyFence(this->device, this->immFence, nullptr);
    Vulkan::vkDestroyCommandPool(this->device, this->immCmdPool, nullptr);

    for (auto& frame : this->frames) {
        frame.frameDescriptors.destroyPools(this->device);

        Vulkan::vkDestroySemaphore(this->device, frame.swapchainSemaphore, nullptr);
        Vulkan::vkDestroySemaphore(this->device, frame.renderSemaphore, nullptr);
        Vulkan::vkDestroyFence(this->device, frame.fence, nullptr);

        Vulkan::vkDestroyCommandPool(this->device, frame.pool, nullptr);
    }
}

void VKRenderer::initPipelines() {
    {
        const auto vert = b::embed<"shaders/vulkan/position.vert.spv">();
        const auto frag = b::embed<"shaders/vulkan/position.frag.spv">();

        this->defaultShaders[0] = Renderer::loadShader(vert, frag, VertexFormats::POSITION);
    }

    {
        const auto vert = b::embed<"shaders/vulkan/position_color.vert.spv">();
        const auto frag = b::embed<"shaders/vulkan/position_color.frag.spv">();

        this->defaultShaders[1] = Renderer::loadShader(vert, frag, VertexFormats::POSITION_COLOR);
    }

    {
        const auto vert = b::embed<"shaders/vulkan/position_tex.vert.spv">();
        const auto frag = b::embed<"shaders/vulkan/position_tex.frag.spv">();

        this->defaultShaders[2] = Renderer::loadShader(vert, frag, VertexFormats::POSITION_TEX);
    }

    {
        const auto vert = b::embed<"shaders/vulkan/position_tex_color.vert.spv">();
        const auto frag = b::embed<"shaders/vulkan/position_tex_color.frag.spv">();

        this->defaultShaders[3] = Renderer::loadShader(vert, frag, VertexFormats::POSITION_TEX_COLOR);
    }
}

void VKRenderer::destroyPipelines() {
    for (auto& pipeline : this->shaders) {
        pipeline.destroy(this->device, nullptr);
    }

    this->shaders.clear();
}

void VKRenderer::initMatrices() {
    this->projectionMatrix = glm::orthoRH_ZO(0.f, static_cast<float>(this->swapchainExtent.width), 0.f, static_cast<float>(this->swapchainExtent.height), 500.f, 0.05f);
    this->viewMatrix = glm::mat4{1.f};
}

void VKRenderer::destroyMeshes() {
    for (const auto& mesh : this->meshes) {
        VK::BufferUtils::destroyBuffer(this->device, nullptr, mesh->vertexBuffer);
        VK::BufferUtils::destroyBuffer(this->device, nullptr, mesh->indexBuffer);
    }

    this->meshes.clear();
}

void VKRenderer::initTextures() {
    const auto black = glm::packUnorm4x8({0, 0, 0, 1});
    const auto magenta = glm::packUnorm4x8({1, 0, 1, 1});

    std::array<uint32_t, 16 * 16> errorPixels{};

    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            errorPixels[y * 16 + x] = (x >> 3) % 2 ^ (y >> 3) % 2 ? magenta : black;
        }
    }

    const auto id = this->loadTexture(errorPixels.data(), errorPixels.size(), 16, 16);

    this->missingTexture = {id, &this->textures[id]};

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.pNext = nullptr;

    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;

    VK_CHECK(vkCreateSampler, this->device, &samplerInfo, nullptr, &this->nearestSampler);

    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    VK_CHECK(vkCreateSampler, this->device, &samplerInfo, nullptr, &this->linearSampler);
}

void VKRenderer::destroyTextures() {
    Vulkan::vkDestroySampler(this->device, this->nearestSampler, nullptr);
    Vulkan::vkDestroySampler(this->device, this->linearSampler, nullptr);

    for (auto& texture : this->textures) {
        this->destroyImage(texture);
    }

    this->textures.clear();
}

void VKRenderer::choosePhysicalDevice() {
    uint32_t physicalDeviceCount = 0;
    VK_CHECK_N(1, vkEnumeratePhysicalDevices, this->instance, &physicalDeviceCount, nullptr);

    std::vector<VkPhysicalDevice> physicalDevices{physicalDeviceCount};
    VK_CHECK_N(2, vkEnumeratePhysicalDevices, this->instance, &physicalDeviceCount, physicalDevices.data());

    std::vector<VkPhysicalDevice> candidates{};
    candidates.reserve(physicalDevices.size());

    for (const auto dev : physicalDevices) {
        {
            uint32_t queueFamilyCount = 0;
            Vulkan::vkGetPhysicalDeviceQueueFamilyProperties2(dev, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties2> queueFamilyProperties{queueFamilyCount};

            for (auto& property : queueFamilyProperties) {
                property.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
            }

            Vulkan::vkGetPhysicalDeviceQueueFamilyProperties2(dev, &queueFamilyCount, queueFamilyProperties.data());

            bool found = false;

            for (const auto& property : queueFamilyProperties) {
                if ((property.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
                    found = true;
                    break;
                }
            }

            if (!found)
                continue;
        }

        {
            uint32_t extensionCount = 0;
            VK_CHECK_N(1, vkEnumerateDeviceExtensionProperties, dev, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> extensions{extensionCount};
            VK_CHECK_N(2, vkEnumerateDeviceExtensionProperties, dev, nullptr, &extensionCount, extensions.data());

            bool found = false;

            for (const auto& property : extensions) {
                if (memcmp(property.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME, strlen(property.extensionName)) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found)
                continue;
        }

        VkPhysicalDeviceProperties2 properties{};
        properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

        Vulkan::vkGetPhysicalDeviceProperties2(dev, &properties);

        if (const auto apiMinor = VK_API_VERSION_MINOR(properties.properties.apiVersion); apiMinor < 3)
            continue;

        if (properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            candidates.insert(candidates.begin(), dev);
        else
            candidates.push_back(dev);
    }

    if (candidates.empty())
        return;

    this->physicalDevice = candidates[0];

    this->memoryProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;

    Vulkan::vkGetPhysicalDeviceMemoryProperties2(this->physicalDevice, &this->memoryProperties);

    {
        uint32_t queueFamilyCount = 0;
        Vulkan::vkGetPhysicalDeviceQueueFamilyProperties2(this->physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties2> queueFamilyProperties{queueFamilyCount};

        for (auto& property : queueFamilyProperties) {
            property.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        }

        Vulkan::vkGetPhysicalDeviceQueueFamilyProperties2(this->physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if ((queueFamilyProperties[i].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
                this->graphicsQueueFamily = i;
                break;
            }
        }
    }
}

VK::BufferedImage VKRenderer::createImage(const VkExtent3D& size, VkFormat format, VkImageUsageFlags usage, bool mipmapped) const {
    VK::BufferedImage image{};
    image.imageFormat = format;
    image.imageExtent = size;

    uint32_t mipLevels = 1;

    if (mipmapped)
        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;

    VK::ImageUtils::allocateImage(this->device, nullptr, size.width, size.height, format, VK_SAMPLE_COUNT_1_BIT, usage, this->memoryProperties.memoryProperties, &image.image, &image.memory, mipLevels);

    VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;

    if (format == VK_FORMAT_D32_SFLOAT)
        aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;

    auto viewInfo = VK::Init::createImageView(image.image, image.imageFormat, aspectFlag);
    viewInfo.subresourceRange.levelCount = mipLevels;

    VK_CHECK(vkCreateImageView, this->device, &viewInfo, nullptr, &image.imageView);

    return image;
}

VK::BufferedImage VKRenderer::createImage(const void* data, const VkExtent3D& size, const VkFormat format, const VkImageUsageFlags usage, const bool mipmapped) const {
    const size_t dataSize = size.depth * size.width * size.height * 4;
    VK::Buffer uploadBuffer{};
    VK::BufferUtils::createBuffer(this->device, nullptr, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->memoryProperties, uploadBuffer);

    void* map = nullptr;
    VK_CHECK(vkMapMemory, this->device, uploadBuffer.memory, 0, dataSize, 0, &map);

    memcpy(map, data, dataSize);

    const auto newImage = this->createImage(size, format, usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, mipmapped);

    this->executeImm([&](VkCommandBuffer cmd) {
        VK::ImageUtils::transitionImage(cmd, newImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_2_NONE, VK_PIPELINE_STAGE_2_COPY_BIT);

        VkBufferImageCopy2 copy{};
        copy.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2;
        copy.pNext = nullptr;

        copy.bufferOffset = 0;
        copy.bufferRowLength = 0;
        copy.bufferImageHeight = 0;

        copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy.imageSubresource.mipLevel = 0;
        copy.imageSubresource.baseArrayLayer = 0;
        copy.imageSubresource.layerCount = 1;
        copy.imageOffset = {};
        copy.imageExtent = size;

        VkCopyBufferToImageInfo2 info{};
        info.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2;
        info.pNext = nullptr;

        info.srcBuffer = uploadBuffer.buffer;
        info.dstImage = newImage.image;
        info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        info.regionCount = 1;
        info.pRegions = &copy;

        Vulkan::vkCmdCopyBufferToImage2(cmd, &info);

        VK::ImageUtils::transitionImage(cmd, newImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_2_COPY_BIT, VK_PIPELINE_STAGE_2_NONE);
    });

    VK::BufferUtils::destroyBuffer(this->device, nullptr, uploadBuffer);

    return newImage;
}

void VKRenderer::destroyImage(const VK::BufferedImage& image) const {
    Vulkan::vkDestroyImageView(this->device, image.imageView, nullptr);
    Vulkan::vkFreeMemory(this->device, image.memory, nullptr);
    Vulkan::vkDestroyImage(this->device, image.image, nullptr);
}

void VKRenderer::executeImm(std::function<void(VkCommandBuffer)>&& func) const {
    VK_CHECK(vkResetFences, this->device, 1, &this->immFence);

    VK_CHECK(vkResetCommandBuffer, this->immCmdBuffer, 0);

    const auto beginInfo = VK::Init::createCommandBufferBegin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer, this->immCmdBuffer, &beginInfo);

    func(this->immCmdBuffer);

    VK_CHECK(vkEndCommandBuffer, this->immCmdBuffer);

    const auto cmdInfo = VK::Init::createCommandBufferSubmit(this->immCmdBuffer);
    const auto submitInfo = VK::Init::createSubmitInfo(&cmdInfo, nullptr, nullptr);

    VK_CHECK(vkQueueSubmit2, this->graphicsQueue, 1, &submitInfo, this->immFence);

    VK_CHECK(vkWaitForFences, this->device, 1, &this->immFence, VK_TRUE, UINT64_MAX);
}

VKRenderer::FrameData& VKRenderer::getCurrentFrame() {
    return this->frames[this->frameNumber % BUFFER_COUNT];
}

VkBool32 VKRenderer::debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) != 0)
        std::cout << "BorionVKDebug - [INFO] " << pCallbackData->pMessage << std::endl;
    else if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0)
        std::cerr << "BorionVKDebug - [WARN] " << pCallbackData->pMessage << std::endl;
    else if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0)
        std::cerr << "BorionVKDebug - [ERR] " << pCallbackData->pMessage << std::endl;
    else
        std::cout << "BorionVKDebug - [DEBUG] " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
