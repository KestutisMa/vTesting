#pragma once
//#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
//#include "VulkanDynamic.hpp"
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <fstream>

#include "context.hpp"
#include "MyBuffer.hpp"
#include "MyTextureImage.hpp"


struct Vertex {
    glm::vec4 pos; //del alignment - vec4 vietoj vec3
    glm::vec4 color;
    glm::vec2 texCoord;

    static vk::VertexInputBindingDescription getBindingDescription()
    {   return vk::VertexInputBindingDescription{0, sizeof(Vertex), vk::VertexInputRate::eVertex}; }

    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions()
    {   return std::array<vk::VertexInputAttributeDescription,3>{ vk::VertexInputAttributeDescription{0,0,vk::Format::eR32G32B32A32Sfloat,offsetof(Vertex, pos)},
                                                                  vk::VertexInputAttributeDescription{1,0,vk::Format::eR32G32B32A32Sfloat,offsetof(Vertex, color)},
                                                                  vk::VertexInputAttributeDescription{2,0,vk::Format::eR32G32Sfloat,offsetof(Vertex, texCoord)}}; }
};

class MyShape {
private:
    vk::Buffer *storageBuffer; // for drawing elements values
    glm::ivec3 gridSize;

    Context *ctx;
    vk::DebugMarkerObjectNameInfoEXT info;
    vk::CommandBufferInheritanceInfo inherInfo;
//    vk::RenderPass *parentRenderPass;
//    vk::SubpassDescription *parentSubpass;
    std::vector<vk::UniqueFramebuffer> *parentFrameBuffers;
    vk::UniqueCommandPool commandPool;
    std::vector<vk::UniqueCommandBuffer> commandBuffers;
    vk::UniqueShaderModule vertexShaderModule;
    vk::UniqueShaderModule fragmentShaderModule;
    std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStages;
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
    vk::UniquePipelineLayout pipelineLayout;
    vk::UniquePipeline pipeline;


    std::vector<Vertex> vertices;
    std::vector<uint32_t> vertexIndices;
//    std::vector<Vertex> vertices = {
//                {{0.f, -0.5f, 0.f, 0.f}, {1.0f, 0.0f, 0.0f, 0.f}},
//                {{ 0.f, 0.5f, 0.f, 0.f}, {0.0f, 1.0f, 0.0f, 0.f}},
//                {{ -0.5f, -0.5f, 0.f, 0.f}, {0.0f, 0.0f, 1.0f, 0.f}},
//                {{ -0.5f, 0.5f, 0.f, 0.f}, {0.0f, 0.0f, 1.0f, 0.f}},
////        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
////        {{ 0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
////        {{ 0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
////        {{-0.5f,  0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
//    };
//    std::vector<uint16_t> vertexIndices = {
////        0, 1, 2, 2, 3, 0
//        0,1,2, 1,0,3
//    };

    std::unique_ptr<MyBuffer> stagingVertexBuffer;
    std::unique_ptr<MyBuffer> tmpHostBuf;
    std::unique_ptr<MyBuffer> vertexBuffer;
    std::unique_ptr<MyBuffer> vertexIndexBuffer;
    std::unique_ptr<MyTextureImage> myTextureImage;
    std::vector<std::unique_ptr<MyBuffer>> uniformBuffers; // one buffer per image (used in command pools)


    //Uniform buffer, for model-projection-view(MPV) transformations
    struct UniformBufferStruct {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    vk::UniqueDescriptorSetLayout descriptorSetLayout;
    vk::UniqueDescriptorPool descriptorPool;
    std::vector<vk::UniqueDescriptorSet> descriptorSets; //butinai vector, nes yra tik ->allocate...Desc..Sets(!)

    std::vector<char> readFile(const std::string &filename);
    void createVertexBuffer();
    void createVertexIndexBuffer();
    void createUniformBuffers();
    void createDescriptorSets();
    void createShaders();
    void createCommandPool();
    void createSecondaryCommandBuffers();
    void recordSecondaryCommandBuffers(); //secondary
    void createPipleine();
public:
    MyShape(Context *ctx, vk::CommandBufferInheritanceInfo inherInfo, std::vector<Vertex> vertices, std::vector<uint32_t> vertexIndices, vk::Buffer *storageBuffer, glm::ivec3 gridSize);//vk::RenderPass *parentRenderPass);//, std::vector<vk::UniqueFramebuffer> *parentFrameBuffers);//, vk::SubpassDescription *parentSubpass);
    ~MyShape() {};
    MyBuffer* getVertexBuffer() {return vertexBuffer.get();};
    vk::CommandBuffer *getSecondaryCommandBuffer(uint32_t i) {return &commandBuffers[i].get();};
    MyTextureImage *getMyTextureImage() {return myTextureImage.get();};
    void updateUniformBuffer(size_t currentFrame, bool autoRotateFlag, glm::vec3 rotationAngles, float fov);
};
