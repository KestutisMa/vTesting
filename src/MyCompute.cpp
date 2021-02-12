#include "MyCompute.hpp"

MyCompute::MyCompute(Context *ctx, std::vector<StorageBufferStruct> storageBufferHostData)
    : ctx(ctx), storageBufferHostData(storageBufferHostData)
{
    commandPool = ctx->device->createCommandPoolUnique(vk::CommandPoolCreateInfo{ vk::CommandPoolCreateFlagBits::eResetCommandBuffer, static_cast<uint32_t>(ctx->computeQueueFamilyIndex) });
    commandBuffers = ctx->device->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(commandPool.get(), vk::CommandBufferLevel::ePrimary, 1));
    fence = ctx->device->createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
    createShaderStorageBuffers();
    createDescriptorSets();
//    bindVertexBufferToComputeShader(vertexBuffer);
    initComputePipeline();
    recordComputeCommandBuffer();
}

void MyCompute::createShaderStorageBuffers()
{
    auto size = storageBufferHostData.size() * sizeof(storageBufferHostData[0]); //buffer size
//    auto size = storageBufferHostData.size(); //buffer size

//    vk::UniqueBuffer stagingBufferTmp;
//    vk::UniqueDeviceMemory stagingDeviceMemoryTmp;

    MyBuffer stagingBufferTmp = MyBuffer(ctx, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    ctx->device->setDebugUtilsObjectNameEXT(vk::DebugUtilsObjectNameInfoEXT{vk::ObjectType::eBuffer, (uint64_t)static_cast<VkBuffer>(*stagingBufferTmp.getBuffer()), "stagingBufferTmp"});

    // create staging buffer and copy data form host to it
//    createBuffer(stagingBufferTmp, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingDeviceMemoryTmp);
    void* data = stagingBufferTmp.mapMemory();
//    void* data = device->mapMemory(*stagingDeviceMemoryTmp, 0, size);
    memcpy(data, storageBufferHostData.data(), (size_t)size);
    stagingBufferTmp.unmapMemory();
//    device->unmapMemory(*stagingDeviceMemoryTmp);

    //copy staging buffer to DeviceLocal buffer
    storageBuffer = std::unique_ptr<MyBuffer>( new MyBuffer(ctx, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal));
    ctx->device->setDebugUtilsObjectNameEXT(vk::DebugUtilsObjectNameInfoEXT{vk::ObjectType::eBuffer, (uint64_t)static_cast<VkBuffer>(*storageBuffer->getBuffer()), "storageBuffer"});
//    createBuffer(SSBuffer, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, SSBmemory);
    stagingBufferTmp.copyToBuffer(storageBuffer.get());
    //    copyBuffer(stagingBufferTmp, SSBuffer, size);

    // TODO: pakeisti pvz i SSB

    // create descriptor pool
//    auto sz = static_cast<uint32_t>(sizeof (SSBhostData));    
}

void MyCompute::createDescriptorSets()
{
    std::vector<vk::DescriptorPoolSize> dPoolSizes = {vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer,1),};

    descriptorPool = ctx->device->createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1, dPoolSizes));

    //////// shader storage buffer (SSB) descriptor set layout
    std::vector<vk::DescriptorSetLayoutBinding> tmpLayoutBindings = {vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute},}; // descriptor count represents array, is this case 1
    descriptorSetLayout = ctx->device->createDescriptorSetLayoutUnique(vk::DescriptorSetLayoutCreateInfo{{},tmpLayoutBindings});

    //create descriptor sets
//    std::vector<vk::DescriptorSetLayout> layouts(imageViews.size(), descriptorSetLayout.get());
//    descriptorSets.resize(imageViews.size()); //TODO: ar reikia?
//    auto vk::UniqueDescriptorSetLayout layouts[] = { SSBdescriptorSetLayout };
//    std::vector<vk::UniqueDescriptorSetLayout> layouts(1);
//    *layouts[0] = *SSBdescriptorSetLayout;

    descriptorSets = ctx->device->allocateDescriptorSetsUnique(vk::DescriptorSetAllocateInfo{*descriptorPool,1, &descriptorSetLayout.get() });

    std::vector<vk::DescriptorBufferInfo> bufferInfos = {vk::DescriptorBufferInfo{*storageBuffer.get()->getBuffer(),0, VK_WHOLE_SIZE } }; //TODO: vertex buffer size

    std::vector<vk::WriteDescriptorSet> writeDescriptorSets = {vk::WriteDescriptorSet{descriptorSets[0].get(), 0, 0, 1, vk::DescriptorType::eStorageBuffer, {}, &bufferInfos[0], {}}};

    ctx->device->updateDescriptorSets(writeDescriptorSets, 0);
}


void MyCompute::recordComputeCommandBuffer()
{
    commandBuffers[0]->begin(vk::CommandBufferBeginInfo{});
        for (int i = 0; i < 1; i++) {
            vk::MemoryBarrier memBar{vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead};
            commandBuffers[0]->pipelineBarrier(vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eComputeShader, {}, { memBar }, {}, {});

            commandBuffers[0]->bindPipeline(vk::PipelineBindPoint::eCompute, pipeline1.get());
            commandBuffers[0]->bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelineLayout.get(), 0, 1, &descriptorSets[0].get(), 0, {});
            int a = 12;
            commandBuffers[0]->pushConstants(pipelineLayout.get(), vk::ShaderStageFlagBits::eCompute, 0, sizeof (int), &a);
            commandBuffers[0]->dispatch( (uint32_t)ceil(SIZE_X / float(WORKGROUP_SIZE)),
                                         (uint32_t)ceil(SIZE_Y / float(WORKGROUP_SIZE)),
                                         (uint32_t)ceil(SIZE_Z / float(WORKGROUP_SIZE)) );

            commandBuffers[0]->pipelineBarrier(vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eComputeShader, {}, { memBar }, {}, {});
            commandBuffers[0]->bindPipeline(vk::PipelineBindPoint::eCompute, pipeline2.get());
            commandBuffers[0]->dispatch( (uint32_t)ceil(SIZE_X / float(WORKGROUP_SIZE)),
                                         (uint32_t)ceil(SIZE_Y / float(WORKGROUP_SIZE)),
                                         (uint32_t)ceil(SIZE_Z / float(WORKGROUP_SIZE)) );
        }
    commandBuffers[0]->end();
}

void MyCompute::initComputePipeline()
{
//    auto ShaderCode1 = readFile("../comp1.spv");
//    auto ShaderCode2 = readFile("../comp2.spv");
    auto ShaderCode1 = readFile("compShader1.comp.spv");
    auto ShaderCode2 = readFile("compShader2.comp.spv");
    shaderModule1 = ctx->device->createShaderModuleUnique(vk::ShaderModuleCreateInfo{{}, ShaderCode1.size(), reinterpret_cast<uint32_t *>(ShaderCode1.data())});
    shaderModule2 = ctx->device->createShaderModuleUnique(vk::ShaderModuleCreateInfo{{}, ShaderCode2.size(), reinterpret_cast<uint32_t *>(ShaderCode2.data())});

//        shaderModule1 = ctx->device->createShaderModuleUnique(vk::ShaderModuleCreateInfo{{}, ShaderCode1}); //TODO
//        shaderModule2 = ctx->device->createShaderModuleUnique(vk::ShaderModuleCreateInfo{{}, ShaderCode2}); //TODO


    struct SpecializationData {  //specialization constants
        int workGroupSizeX;
        int workGroupSizeY;
        int workGroupSizeZ;
        int sizeX;
        int sizeY;
        int sizeZ;
    } specializationData{WORKGROUP_SIZE,WORKGROUP_SIZE,WORKGROUP_SIZE,SIZE_X,SIZE_Y,SIZE_Z};

    std::array<vk::SpecializationMapEntry,6> specMapEntries = {vk::SpecializationMapEntry{1, offsetof(SpecializationData, workGroupSizeX), sizeof (specializationData.workGroupSizeX)},
                                                               vk::SpecializationMapEntry{2, offsetof(SpecializationData, workGroupSizeY), sizeof (specializationData.workGroupSizeY)},
                                                               vk::SpecializationMapEntry{3, offsetof(SpecializationData, workGroupSizeZ), sizeof (specializationData.workGroupSizeZ)},
                                                               vk::SpecializationMapEntry{4, offsetof(SpecializationData, sizeX), sizeof (specializationData.sizeX)},
                                                               vk::SpecializationMapEntry{5, offsetof(SpecializationData, sizeY), sizeof (specializationData.sizeY)},
                                                               vk::SpecializationMapEntry{6, offsetof(SpecializationData, sizeZ), sizeof (specializationData.sizeZ)}};

    auto specInfo = vk::SpecializationInfo{specMapEntries.size(), specMapEntries.data(), sizeof (specializationData), &specializationData};

    auto pipelineShaderStageCreateInfo1 = vk::PipelineShaderStageCreateInfo{{}, vk::ShaderStageFlagBits::eCompute, *shaderModule1, "main", &specInfo};
    auto pipelineShaderStageCreateInfo2 = vk::PipelineShaderStageCreateInfo{{}, vk::ShaderStageFlagBits::eCompute, *shaderModule2, "main", &specInfo};

    std::array<vk::PushConstantRange,1> pushConstRanges = {vk::PushConstantRange{vk::ShaderStageFlagBits::eCompute, 0, sizeof (int)} };
    pipelineLayout = ctx->device->createPipelineLayoutUnique(vk::PipelineLayoutCreateInfo{{},1, &descriptorSetLayout.get(), pushConstRanges.size(), pushConstRanges.data()}); // The pipeline layout allows the pipeline to access descriptor sets. So we just specify the descriptor set layout we created earlier.

    auto computePipelineCreateInfo1 = vk::ComputePipelineCreateInfo{{},pipelineShaderStageCreateInfo1,pipelineLayout.get()};
    auto computePipelineCreateInfo2 = vk::ComputePipelineCreateInfo{{},pipelineShaderStageCreateInfo2,pipelineLayout.get()};

    pipeline1 = ctx->device->createComputePipelineUnique({}, computePipelineCreateInfo1).value;
    pipeline2 = ctx->device->createComputePipelineUnique({}, computePipelineCreateInfo2).value;
}


void MyCompute::getbackSSB()
{
    auto size = sizeof(storageBufferHostData); //buffer size

//    vk::UniqueBuffer stagingBufferTmp;
//    vk::UniqueDeviceMemory stagingDeviceMemoryTmp;

    // create staging buffer
    MyBuffer stagingBufferTmp = MyBuffer(ctx, size, vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
//    createBuffer(stagingBufferTmp, size, vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingDeviceMemoryTmp);

    //copy DeviceLocal buffer to staging buffer
    storageBuffer->copyToBuffer(&stagingBufferTmp);
//    copyBuffer(SSBuffer, stagingBufferTmp, size);

    //copy from staging back to host
    void* data = stagingBufferTmp.mapMemory();
//    void* data = device->mapMemory(*stagingDeviceMemoryTmp, 0, size);
    memcpy(storageBufferHostData.data(), data, (size_t)size);
    stagingBufferTmp.unmapMemory();
//    device->unmapMemory(*stagingDeviceMemoryTmp);
//    return &storageBufferHostData;
}

void MyCompute::runComputeCommandBuffer()
{
    ctx->device->waitForFences(1, &fence.get(), true, std::numeric_limits<uint64_t>::max());
    ctx->device->resetFences(1, &fence.get());

    auto submitInfo = vk::SubmitInfo{ {}, {}, commandBuffers[0].get(), {} };

    ctx->computeQueue.submit(submitInfo, fence.get());

//    ctx->device->waitForFences(1, &fence.get(), true, std::numeric_limits<uint64_t>::max());
//    ctx->device->resetFences(1, &fence.get());
}


std::vector<char> MyCompute::readFile(const std::string &filename) {
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
