#pragma once

#include "../Renderer.h"
#include "descriptor/DescriptorAllocatorGrowable.h"
#include "pipeline/Pipeline.h"
#include "tessellation/Mesh.h"
#include "utils/BufferedImage.h"
#include "utils/Vertex.h"

class VKRenderer : public Renderer {
public:
    struct DrawData {
        VkCommandBuffer cmd{};
        VK::Pipeline* pipeline{};
        glm::mat4 matrix{};
        glm::vec4 shaderColor{};
    };

    VKRenderer(const std::shared_ptr<Win32Window>& window, const std::shared_ptr<RendererContext>& context) : Renderer(window, context) {}

    bool init() override;
    void exit() override;
    void render(double deltaTime) override;
    Type getRendererType() override;
    uint32_t loadShader(const void* vertexShaderData, size_t vertexShaderSize, const void* fragmentShaderData, size_t fragmentShaderSize, std::span<std::string> attributes) override;
    std::shared_ptr<Mesh> upload(float* vertices, uint32_t vertexCount, uint16_t* indices, uint32_t indexCount, uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes) override;
    uint32_t loadTexture(const void* textureData, size_t textureSize, uint32_t width, uint32_t height) override;
    void draw(std::shared_ptr<Mesh> mesh) override;
    void draw(std::shared_ptr<Mesh> mesh, uint32_t texId) override;

    void uploadMeshRaw(std::span<VK::Vertex> vertices, const uint16_t* indices, uint32_t indexCount, VK::Mesh& mesh) const;

private:
    struct FrameData {
        VkCommandPool pool{};
        VkCommandBuffer cmd{};

        VkFence fence{};
        VkSemaphore swapchainSemaphore{};
        VkSemaphore renderSemaphore{};

        VK::DescriptorAllocatorGrowable frameDescriptors{};
    };

    static constexpr uint32_t BUFFER_COUNT = 2;

    VkInstance instance{};
    VkPhysicalDevice physicalDevice{};
    VkPhysicalDeviceMemoryProperties2 memoryProperties{};
    VkDevice device{};
    VkSurfaceKHR surface{};
    VkDebugUtilsMessengerEXT debugMessenger{};
    VkQueue graphicsQueue{};

    VkSwapchainKHR swapchain{};
    VkFormat swapchainImageFormat{};
    bool resizeRequested = false;

    std::vector<VkImage> swapchainImages{};
    std::vector<VkImageView> swapchainImageViews{};
    VkExtent2D swapchainExtent{};

    FrameData frames[BUFFER_COUNT]{};
    uint8_t frameNumber = 0;

    uint32_t graphicsQueueFamily{};

    VK::BufferedImage backBuffer{};
    VK::BufferedImage depthImage{};

    VkDescriptorSetLayout singleImageDescriptorLayout{};

    VkFence immFence{};
    VkCommandPool immCmdPool{};
    VkCommandBuffer immCmdBuffer{};

    VkSampler linearSampler{};
    VkSampler nearestSampler{};

    std::array<uint32_t, 4> defaultShaders{};
    std::vector<VK::Pipeline> shaders{};

    std::vector<std::shared_ptr<VK::Mesh>> meshes{};

    std::pair<uint32_t, VK::BufferedImage*> missingTexture{};
    std::vector<VK::BufferedImage> textures{};

    void initInstance();
    void destroyInstance() const;

    void initMessenger();
    void destroyMessenger() const;

    bool initDevice();
    void destroyDevice() const;

    void initSwapchain(uint32_t width, uint32_t height);
    void destroySwapchain() const;
    void recreateSwapchain(uint32_t width, uint32_t height);

    void allocateBackBuffer();
    void freeBackBuffer() const;

    void initDescriptors();
    void destroyDescriptors() const;

    void initCommands();
    void initSyncStructures();

    void destroyFrameData();

    void initPipelines();
    void destroyPipelines();

    void initMatrices();

    void destroyMeshes();

    void initTextures();
    void destroyTextures();

    void choosePhysicalDevice();

    [[nodiscard]] VK::BufferedImage createImage(const VkExtent3D& size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false) const;
    [[nodiscard]] VK::BufferedImage createImage(const void* data, const VkExtent3D& size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false) const;
    void destroyImage(const VK::BufferedImage& image) const;

    void executeImm(std::function<void(VkCommandBuffer)>&& func) const;

    FrameData& getCurrentFrame();

    static VkBool32 debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};
