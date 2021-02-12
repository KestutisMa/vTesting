#pragma once
//#include <vulkan/vulkan.hpp>
#include "context.hpp"

class MyBuffer {
private:
    Context *ctx;
    vk::DeviceSize size;
    vk::UniqueCommandPool commandPool;
    vk::UniqueBuffer buffer;
    vk::UniqueDeviceMemory bufferMemory;
//    vk::UniqueBuffer stagingBuffer;
//    vk::UniqueDeviceMemory stagingBufferMemory;
public:
    MyBuffer(Context *ctx, vk::DeviceSize size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags, vk::SharingMode sharingMode = vk::SharingMode::eExclusive);
    ~MyBuffer();
    vk::Buffer *getBuffer() {return &buffer.get();};
//    vk::DeviceSize getSize() { return size;}
    void *mapMemory();
    void unmapMemory();
    void copyToBuffer(MyBuffer *dstBuffer);
    static uint32_t findMemoryType(Context *ctx, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
};

//class MyHostSharedBuffer: public MyBuffer {

//public:
//    MyHostSharedBuffer(Context *ctx, vk::DeviceSize size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags, vk::SharingMode sharingMode = vk::SharingMode::eExclusive);
//    ~MyHostSharedBuffer();
//    void *getHostMappedPointer();
//};
