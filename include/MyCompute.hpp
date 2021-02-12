#pragma once

#include "MyBuffer.hpp"
#include "MyTextureImage.hpp"
#include <math.h>
#include <fstream>
#include <glm/glm.hpp>

struct alignas(16) StorageBufferStruct { //shader storage buffer, (visi glsl vec tipai 16bytes aligned)
//    float fin[9];
    float   rho; // kintamasis negali prasideti vienoje 16 bytes puseje, o pasibaigti kitoje
//    glm::vec2   u;
//    float fout[9];
};

//compute shader
class MyCompute {
    int SIZE_X = 1;
    int SIZE_Y = 1;
    int SIZE_Z = 1;
    int WORKGROUP_SIZE = 1; // Workgroup size in compute shader. FIXME: Kazkodel layers meta fence wait klaida kai 32

//    size_t N = SIZE_X*SIZE_Y*SIZE_Z;
    std::vector<StorageBufferStruct> storageBufferHostData; //shader storage buffer
    Context *ctx;
    std::unique_ptr<MyBuffer> storageBuffer;
    vk::UniqueDescriptorSetLayout descriptorSetLayout;
    vk::UniqueDescriptorPool descriptorPool;
    std::vector<vk::UniqueDescriptorSet> descriptorSets;
    vk::UniqueShaderModule shaderModule1;
    vk::UniqueShaderModule shaderModule2;
    vk::UniquePipeline pipeline1;
    vk::UniquePipeline pipeline2;
    vk::UniquePipelineLayout pipelineLayout;
    vk::UniqueCommandPool commandPool;
    std::vector<vk::UniqueCommandBuffer> commandBuffers;
    vk::UniqueFence fence;

    static std::vector<char> readFile(const std::string &filename);
public:
    MyCompute(Context *ctx, std::vector<StorageBufferStruct> storageBufferHostData);
    void createShaderStorageBuffers();
    void createDescriptorSets();
//    void bindVertexBufferToComputeShader();
    void initComputePipeline();
    void recordComputeCommandBuffer();
    void runComputeCommandBuffer();
    vk::Buffer *getStorageBuffer() {return storageBuffer->getBuffer();};
    void getbackSSB();
};
