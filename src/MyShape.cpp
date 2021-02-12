//#pragma once
//#define GLFW_INCLUDE_VULKAN
//#define GLM_FORCE_RADIANS
//#include <vulkan/vulkan.hpp>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>


#include "MyShape.hpp"

MyShape::MyShape(Context *ctx, vk::CommandBufferInheritanceInfo inherInfo, std::vector<Vertex> vertices, std::vector<uint32_t> vertexIndices,  vk::Buffer *storageBuffer, glm::ivec3 gridSize)
    : ctx(ctx), inherInfo(inherInfo), vertices(vertices), vertexIndices(vertexIndices), storageBuffer(storageBuffer), gridSize(gridSize)
{
    createVertexBuffer();
    createVertexIndexBuffer();
    createUniformBuffers();
    myTextureImage = std::unique_ptr<MyTextureImage>( new MyTextureImage(ctx) );
    createDescriptorSets();
    createShaders();
    createCommandPool();
    createSecondaryCommandBuffers();
    createPipleine();
    recordSecondaryCommandBuffers();
}

void MyShape::createVertexBuffer()
{
    auto size = sizeof(vertices[0]) * vertices.size(); //buffer size

//    vk::UniqueBuffer stagingVertexBuffer;
//    vk::UniqueDeviceMemory stagingVertexBufferMemory;

    //init staging
    stagingVertexBuffer = std::unique_ptr<MyBuffer>( new MyBuffer(ctx, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    void* data = stagingVertexBuffer->mapMemory();
    memcpy(data, vertices.data(), (size_t)size);
    stagingVertexBuffer->unmapMemory();
    //copy staging to real
    vertexBuffer = std::unique_ptr<MyBuffer>( new MyBuffer(ctx, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eStorageBuffer,  vk::MemoryPropertyFlagBits::eDeviceLocal));
    ctx->device->setDebugUtilsObjectNameEXT(vk::DebugUtilsObjectNameInfoEXT{vk::ObjectType::eBuffer, (uint64_t)static_cast<VkBuffer>(*vertexBuffer->getBuffer()), "vertexBuffer"});
    stagingVertexBuffer->copyToBuffer(vertexBuffer.get());

    //old fashion buffer create
//        auto size = sizeof(vertices[0]) * vertices.size();
//    vertexBuffer = device->createBufferUnique(vk::BufferCreateInfo{{}, size, vk::BufferUsageFlagBits::eVertexBuffer, vk::SharingMode::eExclusive });
//    auto memRequirements = device->getBufferMemoryRequirements(*vertexBuffer);
//    vertexBufferMemory = device->allocateMemoryUnique(vk::MemoryAllocateInfo{memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)});
    //    device->bindBufferMemory(*vertexBuffer, *vertexBufferMemory, 0);
}

void MyShape::createVertexIndexBuffer()
{
    auto size = sizeof(vertexIndices[0]) * vertexIndices.size(); //buffer size

//    vk::UniqueBuffer stagingBufferTmp;
//    vk::UniqueDeviceMemory stagingDeviceMemoryTmp;
//    MyBuffer tmpHostBuf(ctx, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    //init staging
//    createBuffer(stagingBufferTmp, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingDeviceMemoryTmp);

    tmpHostBuf = std::unique_ptr<MyBuffer>( new MyBuffer(ctx, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

    info = vk::DebugMarkerObjectNameInfoEXT{vk::DebugReportObjectTypeEXT::eBuffer, (uint64_t)static_cast<VkBuffer>(*tmpHostBuf.get()->getBuffer()), "labasidxStag"};
//    auto info = vk::DebugMarkerObjectNameInfoEXT{vk::DebugReportObjectTypeEXT::eBuffer, reinterpret_cast<uint64_t &>(tmpHostBuf.get()->getBuffer()), "idxStag"};
//    auto dldi = vk::DispatchLoaderDynamic(ctx->instance.get(), vkGetInstanceProcAddr);
//    auto dldi = vk::DispatchLoaderDynamic( ctx->instance.get(), vkGetInstanceProcAddr, ctx->device.get(), vkGetDeviceProcAddr);
//    ctx->device->debugMarkerSetObjectNameEXT(&info, dldi);

//    auto pfnDebugMarkerSetObjectName = (PFN_vkDebugMarkerSetObjectNameEXT)vkGetDeviceProcAddr(static_cast<VkDevice>(ctx->device.get()), "vkDebugMarkerSetObjectNameEXT");
//    VkDebugMarkerObjectNameInfoEXT nameInfo = {};
//    nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
//    // Type of the object to be named
//    nameInfo.objectType = VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT;
//    // Handle of the object cast to unsigned 64-bit integer
//    nameInfo.object = (uint64_t)static_cast<VkBuffer>(*tmpHostBuf.get()->getBuffer());
//    // Name to be displayed in the offline debugging application
//    nameInfo.pObjectName = "Primary Command Buffer";
//    pfnDebugMarkerSetObjectName(static_cast<VkDevice>(ctx->device.get()), &nameInfo);

//    void* data = device->mapMemory(*stagingDeviceMemoryTmp, 0, size);

    void *data = tmpHostBuf->mapMemory();
    memcpy(data, vertexIndices.data(), (size_t)size);
//    device->unmapMemory(*stagingDeviceMemoryTmp);
    tmpHostBuf->unmapMemory();

    //copy staging to real
//    createBuffer(vertexIndexBuffer, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexIndexBufferMemory);
    vertexIndexBuffer = std::unique_ptr<MyBuffer>( new MyBuffer(ctx, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal));
//    copyBuffer(stagingBufferTmp, vertexIndexBuffer, size);
    tmpHostBuf->copyToBuffer(vertexIndexBuffer.get());
}

void MyShape::createUniformBuffers()
{
    vk::DeviceSize bufferSize = sizeof(UniformBufferStruct);
    uniformBuffers.resize(ctx->imageCount);

    for (size_t i = 0; i < ctx->imageCount; i++) {
//        createBuffer(uniformBuffers[i], bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffersMemory[i]);
        uniformBuffers[i] = std::unique_ptr<MyBuffer>( new MyBuffer(ctx,  bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    }
}

void MyShape::createDescriptorSets()
{
    //create descriptor set layout
    std::array<vk::DescriptorSetLayoutBinding,2> bindings = {vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex}, // descriptor count represents array, is this case 1
                                                             vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex}}; // our texture

    descriptorSetLayout = ctx->device->createDescriptorSetLayoutUnique(vk::DescriptorSetLayoutCreateInfo{{},bindings});

    // create descriptor pool
    auto sz = static_cast<uint32_t>(ctx->imageCount);

    std::array<vk::DescriptorPoolSize,2> dPoolSizes = {vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer,sz},
                                                       vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer,sz} };

    descriptorPool = ctx->device->createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,sz, dPoolSizes));

    //create descriptor sets
    std::vector<vk::DescriptorSetLayout> layouts(ctx->imageCount, descriptorSetLayout.get());
//    descriptorSets.resize(imageViews.size()); //TODO: ar reikia?
    descriptorSets = ctx->device->allocateDescriptorSetsUnique(vk::DescriptorSetAllocateInfo{*descriptorPool,sz,layouts.data()});
    for (size_t i = 0; i < ctx->imageCount; i++) {
        std::array<vk::DescriptorBufferInfo,2> bufferInfo = {vk::DescriptorBufferInfo{*uniformBuffers[i].get()->getBuffer(),0,sizeof(UniformBufferStruct)},
                                                             vk::DescriptorBufferInfo{*storageBuffer,0, VK_WHOLE_SIZE}};

        std::array<vk::WriteDescriptorSet,2> writeDescriptorSets = {vk::WriteDescriptorSet{*descriptorSets[i], 0, 0, vk::DescriptorType::eUniformBuffer, {}, bufferInfo[0]},
                                                                    vk::WriteDescriptorSet{*descriptorSets[i], 1, 0, vk::DescriptorType::eStorageBuffer,  {}, bufferInfo[1]} };
//        std::array<vk::WriteDescriptorSet,1> writeDescriptorSets = {vk::WriteDescriptorSet{*descriptorSets[i], 0, 0, vk::DescriptorType::eUniformBuffer, {}, bufferInfo, {}}};
        ctx->device->updateDescriptorSets(writeDescriptorSets, {});
    }
}


void MyShape::updateUniformBuffer(size_t currentFrame, bool autoRotateFlag, glm::vec3 rotationAngles, float fov)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferStruct ubo{};
//    ubo.foo = glm::vec2(1.f,2.f);
    if (autoRotateFlag)
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    else {
        ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
        ubo.model = glm::rotate(ubo.model, glm::radians(rotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.model = glm::rotate(ubo.model, glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    ubo.proj = glm::perspective(glm::radians(fov), ctx->extent.width / (float) ctx->extent.height, .01f, 5.0f);

    ubo.proj[1][1] *= -1; //for opengl->vulkan conversion

    void* data = uniformBuffers[currentFrame].get()->mapMemory(); //ctx->device->mapMemory(*uniformBuffersMemory[currentFrame], 0, sizeof(ubo));
    memcpy(data, &ubo, sizeof(ubo));
    uniformBuffers[currentFrame].get()->unmapMemory(); //    device->unmapMemory(*uniformBuffersMemory[currentFrame]);
}

void MyShape::createShaders() {
  auto vertShaderCode = readFile("vertShader.vert.spv");

//  auto vertShaderCreateInfo = vk::ShaderModuleCreateInfo{
//      {}, vertShaderCode.size(), vertShaderCode.data()};

//  auto vertShaderCreateInfo = vk::ShaderModuleCreateInfo{
//      {}, vertShaderCode};

  vertexShaderModule =
      ctx->device->createShaderModuleUnique(vk::ShaderModuleCreateInfo{{}, vertShaderCode.size(), reinterpret_cast<uint32_t *>(vertShaderCode.data())});
//          ctx->device->createShaderModuleUnique(vk::ShaderModuleCreateInfo{{}, vertShaderCode});

  auto fragShaderCode = readFile("fragShader.frag.spv");

  auto fragShaderCreateInfo = vk::ShaderModuleCreateInfo{
      {}, fragShaderCode.size(), reinterpret_cast<uint32_t *>(fragShaderCode.data())};

  fragmentShaderModule =
      ctx->device->createShaderModuleUnique(fragShaderCreateInfo);

  //specialization constants
  struct SpecializationData {  //specialization constants
      int sizeX;
      int sizeY;
      int sizeZ;
  } specializationData{gridSize.x,gridSize.y,gridSize.z};

  std::array<vk::SpecializationMapEntry,3> specMapEntries = {vk::SpecializationMapEntry{0, offsetof(SpecializationData, sizeX), sizeof (specializationData.sizeX)},
                                                             vk::SpecializationMapEntry{1, offsetof(SpecializationData, sizeY), sizeof (specializationData.sizeY)},
                                                             vk::SpecializationMapEntry{2, offsetof(SpecializationData, sizeZ), sizeof (specializationData.sizeZ)}};

  auto specInfo = vk::SpecializationInfo{specMapEntries.size(), specMapEntries.data(), sizeof (specializationData), &specializationData};


  auto vertShaderStageInfo = vk::PipelineShaderStageCreateInfo{
      {}, vk::ShaderStageFlagBits::eVertex, *vertexShaderModule, "main", &specInfo};

  auto fragShaderStageInfo = vk::PipelineShaderStageCreateInfo{
      {}, vk::ShaderStageFlagBits::eFragment, *fragmentShaderModule, "main"};

  pipelineShaderStages = std::vector<vk::PipelineShaderStageCreateInfo>{
          vertShaderStageInfo, fragShaderStageInfo};
  printf("\nMyShape shader done.\n");
}

void MyShape::createCommandPool()
{
    commandPool = ctx->device->createCommandPoolUnique(vk::CommandPoolCreateInfo{ vk::CommandPoolCreateFlagBits::eResetCommandBuffer, static_cast<uint32_t>(ctx->graphicsQueueFamilyIndex) });
}

void MyShape::createSecondaryCommandBuffers()
{
    commandBuffers = ctx->device->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo((commandPool).get(), vk::CommandBufferLevel::eSecondary, ctx->imageCount));

//    auto framebuffers = std::vector<vk::UniqueFramebuffer>(ctx->imageCount);

//    for (size_t i = 0; i < ctx->imageCount; i++) {
//        framebuffers[i] = ctx->device->createFramebufferUnique(vk::FramebufferCreateInfo{
//            {}, parentRenderPass.get(), 1, &imageViews[i].get(), ctx.extent.width, ctx.extent.height, 1 });
//    }
}

void MyShape::recordSecondaryCommandBuffers()
{
    for (size_t i = 0; i < commandBuffers.size(); i++) {
//        vk::CommandBufferInheritanceInfo inherInfo{*parentRenderPass};//, 0, (*parentFrameBuffers)[i].get() };
        commandBuffers[i]->begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inherInfo});
//            vk::ClearValue clearValues{};
//            auto renderPassBeginInfo = vk::RenderPassBeginInfo{ ctx.renderPass.get(), framebuffers[i].get(),
//                vk::Rect2D{ { 0, 0 }, ctx.extent }, 1, &clearValues };
            vk::DeviceSize offsets[] = { 0 };
//            commandBuffers[i]->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
//                commandBuffers[i]->setViewport(0, 1, &ctx->viewport);
//                commandBuffers[i]->setScissor(0,1,&ctx->scissor);
                commandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());
                commandBuffers[i]->bindVertexBuffers(0,1, vertexBuffer.get()->getBuffer(), offsets );
                commandBuffers[i]->bindIndexBuffer(*vertexIndexBuffer.get()->getBuffer(),0,vk::IndexType::eUint32);
                commandBuffers[i]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout.get(), 0, 1, &descriptorSets[i].get(), 0, {});
                commandBuffers[i]->drawIndexed(static_cast<uint32_t>(vertexIndices.size()), 1, 0, 0, 0);
//                  commandBuffers[i]->draw(3, 1, 0, 0);
//            commandBuffers[i]->endRenderPass();

        commandBuffers[i]->end();
    }
}

void MyShape::createPipleine()
{   // vertexInput
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    auto vertexInputInfo =
            vk::PipelineVertexInputStateCreateInfo{{}, 1u, &bindingDescription, static_cast<uint32_t>(attributeDescriptions.size()), attributeDescriptions.data()};

    auto inputAssembly = vk::PipelineInputAssemblyStateCreateInfo{
      {}, vk::PrimitiveTopology::eTriangleList, false};
    /////////////////////////////////////


    ////

    auto rasterizer = vk::PipelineRasterizationStateCreateInfo{ {}, /*depthClamp*/ false,
            /*rasterizeDiscard*/ false, vk::PolygonMode::eFill, /*cullmode*/ {}, //vk::CullModeFlagBits::eBack, //jei nieko nepaiso tai culling mode turi buti eBack/eFront
        /*frontFace*/ vk::FrontFace::eCounterClockwise, {}, {}, {}, {}, 1.0f };

    auto multisampling = vk::PipelineMultisampleStateCreateInfo{ {}, vk::SampleCountFlagBits::e1, false, 1.0 };

    auto colorBlendAttachment = vk::PipelineColorBlendAttachmentState{ {}, /*srcCol*/ vk::BlendFactor::eOne,
        /*dstCol*/ vk::BlendFactor::eZero, /*colBlend*/ vk::BlendOp::eAdd,
        /*srcAlpha*/ vk::BlendFactor::eOne, /*dstAlpha*/ vk::BlendFactor::eZero,
        /*alphaBlend*/ vk::BlendOp::eAdd,
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };

    auto colorBlending = vk::PipelineColorBlendStateCreateInfo{ {}, /*logicOpEnable=*/false,
        vk::LogicOp::eCopy, /*attachmentCount=*/1, /*colourAttachments=*/&colorBlendAttachment };

    auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo{{},1, &descriptorSetLayout.get()};
//    auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo{}; // be buff

    pipelineLayout = ctx->device->createPipelineLayoutUnique(pipelineLayoutInfo, nullptr);

//    auto colorAttachment = vk::AttachmentDescription{ {}, ctx->format, vk::SampleCountFlagBits::e1,
//        vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, {}, {}, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal};//vk::ImageLayout::eColorAttachmentOptimal }; //kai imgui VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, vk::ImageLayout::ePresentSrcKHR

//    auto colourAttachmentRef = vk::AttachmentReference{ 0, vk::ImageLayout::eColorAttachmentOptimal}; // orig vk::ImageLayout::eColorAttachmentOptimal

//    auto subpass = vk::SubpassDescription{ {}, vk::PipelineBindPoint::eGraphics,
//        /*inAttachmentCount*/ 0, nullptr, 1, &colourAttachmentRef };

//    auto subpassDependency = vk::SubpassDependency{ VK_SUBPASS_EXTERNAL, 0,
//        vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
//        {}, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite };

//    ctx.renderPass = device->createRenderPassUnique(
//        vk::RenderPassCreateInfo{ {}, 1, &colorAttachment, 1, &subpass});//, 1, &subpassDependency });


    auto depthStencil = vk::PipelineDepthStencilStateCreateInfo{{}, true, true, vk::CompareOp::eLess, false, false, {}, {}, /*minDepth*/0.0f, /*max*/1.0f  };

    //su buffers
    auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo{ {}, 2, pipelineShaderStages.data(),
        &vertexInputInfo, &inputAssembly, nullptr, &ctx->viewportState, &rasterizer, &multisampling, &depthStencil,
        &colorBlending, nullptr, pipelineLayout.get(), inherInfo.renderPass, 0 };

    // be buffers
//    auto vertexInputInfo0 =
//            vk::PipelineVertexInputStateCreateInfo{{}, 0, {}, 0, {}};
//    auto inputAssembly0 = vk::PipelineInputAssemblyStateCreateInfo{
//      {}, vk::PrimitiveTopology::eTriangleList, false};
//    auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo{ {}, 2, pipelineShaderStages.data(),
//    &vertexInputInfo0, &inputAssembly0, nullptr, &ctx->viewportState, &rasterizer, &multisampling,
//        nullptr, &colorBlending, nullptr, pipelineLayout.get(), *parentRenderPass, 0 };
    // END be buffers

    pipeline = ctx->device->createGraphicsPipelineUnique({}, pipelineCreateInfo).value;

    //todo perkelti i initCommandBuffers
//    framebuffers = std::vector<vk::UniqueFramebuffer>(ctx.imageCount);
//    for (size_t i = 0; i < imageViews.size(); i++) {
//        framebuffers[i] = device->createFramebufferUnique(vk::FramebufferCreateInfo{
//            {}, ctx.renderPass.get(), 1, &imageViews[i].get(), ctx.extent.width, ctx.extent.height, 1 });
//    }
}

std::vector<char> MyShape::readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }
  // start at and to get size, then go to begining and read all
  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

