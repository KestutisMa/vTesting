#include "MyBuffer.hpp"

MyBuffer::MyBuffer(Context *ctx, vk::DeviceSize size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags, vk::SharingMode sharingMode)
    : ctx{ctx}, size{size}
{
    commandPool = ctx->device->createCommandPoolUnique(vk::CommandPoolCreateInfo{ {}, static_cast<uint32_t>(ctx->transferQueueFamilyIndex) });

    buffer = ctx->device->createBufferUnique(vk::BufferCreateInfo{{}, size, usageFlags, sharingMode});

    auto memRequirements = ctx->device->getBufferMemoryRequirements(*buffer);
    bufferMemory = ctx->device->allocateMemoryUnique(vk::MemoryAllocateInfo{memRequirements.size, findMemoryType(ctx, memRequirements.memoryTypeBits, memoryPropertyFlags)});
    ctx->device->bindBufferMemory(*buffer, *bufferMemory, 0);
}

MyBuffer::~MyBuffer()
{

}

void *MyBuffer::mapMemory()
{
    void* data = ctx->device->mapMemory(bufferMemory.get(), 0, size);
    return data;
}

void MyBuffer::unmapMemory()
{
    ctx->device->unmapMemory(bufferMemory.get());
}

void MyBuffer::copyToBuffer(MyBuffer *dstBuffer)
{
//    const vk::CommandBufferAllocateInfo info[] = {vk::CommandBufferAllocateInfo{*commandPool,vk::CommandBufferLevel::ePrimary,1}};
    std::vector<vk::UniqueCommandBuffer> buf(1); //TODO: gal galima tiesiogiai be vector?
    buf = ctx->device->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(commandPool.get(),vk::CommandBufferLevel::ePrimary,1));

    buf[0]->begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

        vk::BufferCopy copyRegion[] = { vk::BufferCopy{{},{},size} };
        buf[0]->copyBuffer(buffer.get(),*dstBuffer->getBuffer(),1, copyRegion);

    buf[0]->end();

    auto submitInfo = vk::SubmitInfo{{},{},{},1,&buf[0].get()};
    ctx->transferQueue.submit(submitInfo, {});
    ctx->transferQueue.waitIdle();

}

uint32_t MyBuffer::findMemoryType(Context *ctx, uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = ctx->physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

//MyHostSharedBuffer::MyHostSharedBuffer(Context *ctx, vk::DeviceSize size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags, vk::SharingMode sharingMode):
//    MyBuffer{ctx, size, usageFlags, memoryPropertyFlags, sharingMode}
//{

//}

//MyHostSharedBuffer::~MyHostSharedBuffer()
//{
//    ctx->device->unmapMemory(bufferMemory.get());
//}

//void *MyHostSharedBuffer::getHostMappedPointer()
//{
//    void* data = ctx->device->mapMemory(bufferMemory.get(), 0, size);
//    return data;
//}
