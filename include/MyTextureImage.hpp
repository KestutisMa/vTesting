#pragma once
//#include <vulkan/vulkan.hpp>
#include "context.hpp"
#include "MyBuffer.hpp"

class MyTextureImage {
private:
    Context *ctx;
    vk::DeviceSize size;
    vk::UniqueCommandPool commandPool;
//    vk::UniqueBuffer buffer;
//    vk::UniqueDeviceMemory bufferMemory;
    vk::UniqueImage textureImage;
    vk::UniqueDeviceMemory textureImageMemory;
    vk::UniqueImageView textureImageView;
    vk::UniqueSampler textureSampler;

//    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
public:
    MyTextureImage(Context *ctx);//, vk::DeviceSize size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags, vk::SharingMode sharingMode = vk::SharingMode::eExclusive);
//    ~MyTextureImage();
    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();
    vk::Sampler getSampler() {return textureSampler.get();};
    vk::ImageView getImageView() {return textureImageView.get();};
//    vk::Buffer *getBuffer() {return &buffer.get();};
//    vk::DeviceSize getSize() { return size;}
//    void *mapMemory();
//    void unmapMemory();
//    void copyToBuffer(MyBuffer *dstBuffer);
};

//class MyHostSharedBuffer: public MyBuffer {

//public:
//    MyHostSharedBuffer(Context *ctx, vk::DeviceSize size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags, vk::SharingMode sharingMode = vk::SharingMode::eExclusive);
//    ~MyHostSharedBuffer();
//    void *getHostMappedPointer();
//};
