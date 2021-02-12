#include "MyTextureImage.hpp"

MyTextureImage::MyTextureImage(Context *ctx): ctx{ctx}
{
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
}

//MyTextureImage::~MyTextureImage()
//{

//}

void MyTextureImage::createTextureImage() {
    const int texWidth = 10, texHeight = 10;


    struct im {
        GLfloat r;
        GLfloat g;
        GLfloat b;
        GLfloat a;
    } initIm[texWidth * texHeight] = {};

    vk::DeviceSize imageSize = texWidth * texHeight * sizeof(im);
//    float a[texWidth * texHeight] = {};
    for (int i = 0; i < texWidth * texHeight; i++) {
        initIm[i] = im{static_cast<GLfloat>(0.0), static_cast<GLfloat>(0), static_cast<GLfloat>(1.0), static_cast<GLfloat>(1.0)};
    }
    void *pixels = &initIm; //TODO

//    vk::UniqueBuffer stagingBuffer;
//    vk::UniqueDeviceMemory stagingBufferMemory;

    // create staging buffer
    MyBuffer stagingBuffer(ctx, imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void* data = stagingBuffer.mapMemory();
    memcpy(data, pixels, imageSize);
    stagingBuffer.unmapMemory();

    //create image
    auto imageInfo = vk::ImageCreateInfo{{}, vk::ImageType::e2D, vk::Format::eR32G32B32A32Sfloat, vk::Extent3D{static_cast<uint32_t>(texWidth),static_cast<uint32_t>(texHeight),1},1,1,
                        vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlags{vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage},
                        vk::SharingMode::eExclusive, {}, vk::ImageLayout::eUndefined};
    textureImage = ctx->device->createImageUnique(imageInfo);

    auto memRequirements = ctx->device->getImageMemoryRequirements(textureImage.get());
    textureImageMemory = ctx->device->allocateMemoryUnique(vk::MemoryAllocateInfo{memRequirements.size, MyBuffer::findMemoryType(ctx, memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)});
    ctx->device->bindImageMemory(textureImage.get(), textureImageMemory.get(), 0);

    //copy stagingBuffer to image
    //    const vk::CommandBufferAllocateInfo info[] = {vk::CommandBufferAllocateInfo{*commandPool,vk::CommandBufferLevel::ePrimary,1}};
        std::vector<vk::UniqueCommandBuffer> buf(1); //TODO: gal galima tiesiogiai be vector?
        commandPool = ctx->device->createCommandPoolUnique(vk::CommandPoolCreateInfo{ {}, static_cast<uint32_t>(ctx->transferQueueFamilyIndex) }); //TODO: transfer?
        buf = ctx->device->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(commandPool.get(),vk::CommandBufferLevel::ePrimary,1));

        buf[0]->begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

            vk::ImageMemoryBarrier barrier1{{}, vk::AccessFlagBits::eTransferWrite, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, textureImage.get(),
                                        vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};
            buf[0]->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,{}, {}, {}, barrier1);

            vk::BufferImageCopy copyRegion = { vk::BufferImageCopy{0, 0, 0, vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1}, vk::Offset3D{0, 0, 0}, vk::Extent3D{static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1}} };
            buf[0]->copyBufferToImage(*stagingBuffer.getBuffer(),textureImage.get(), vk::ImageLayout::eTransferDstOptimal, 1, &copyRegion);

            vk::ImageMemoryBarrier barrier2{vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead,  vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eGeneral/*eShaderReadOnlyOptimal*/, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, textureImage.get(),
                                        vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};
            buf[0]->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,{}, {}, {}, barrier2);

        buf[0]->end();

        auto submitInfo = vk::SubmitInfo{{},{},{},1,&buf[0].get()};
        ctx->transferQueue.submit(submitInfo, {});
        ctx->transferQueue.waitIdle();
}

void MyTextureImage::createTextureImageView() {
    auto info = vk::ImageViewCreateInfo{{}, textureImage.get(), vk::ImageViewType::e2D, vk::Format::eR32G32B32A32Sfloat, {}, vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1} };
    textureImageView = ctx->device->createImageViewUnique(info);
}

void MyTextureImage::createTextureSampler()
{
    auto info = vk::SamplerCreateInfo{{}, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear,
            vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, 0.0f,
            VK_TRUE, ctx->physicalDevice.getProperties().limits.maxSamplerAnisotropy,
            VK_FALSE, vk::CompareOp::eAlways, 0.0f, 0.0f, vk::BorderColor::eFloatOpaqueBlack, VK_FALSE};

    textureSampler = ctx->device->createSamplerUnique(info);
}


//void *MyBuffer::mapMemory()
//{
//    void* data = ctx->device->mapMemory(bufferMemory.get(), 0, size);
//    return data;
//}

//void MyBuffer::unmapMemory()
//{
//    ctx->device->unmapMemory(bufferMemory.get());
//}

