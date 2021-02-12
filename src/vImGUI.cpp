#include "vImGUI.hpp"

vImGUI::vImGUI( GLFWwindow *window, Context *ctx) : window{window}, ctx{ctx}
{
    createCommandPool();
    createRenderPass();
    createFrameBuffers();
    init();
    createCommandBuffers();
}

vImGUI::~vImGUI()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ctx->device->freeCommandBuffers(commandPool.get(),commandBuffers);
}

void vImGUI::init() {
//    createCommandBuffers(frameBufferSize);
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Create Descriptor Pool
    {
        vk::DescriptorPoolSize pool_sizes[] =
        {
            vk::DescriptorPoolSize{vk::DescriptorType::eSampler, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageImage, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eUniformTexelBuffer, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageTexelBuffer, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eUniformBufferDynamic, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageBufferDynamic, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eInputAttachment, 1000}};
        descriptorPool = ctx->device->createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,1000 * IM_ARRAYSIZE(pool_sizes), (uint32_t)IM_ARRAYSIZE(pool_sizes), pool_sizes));
    }

//    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
//    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForVulkan(window, true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = ctx->instance.get();
    init_info.PhysicalDevice = ctx->physicalDevice;
    init_info.Device = ctx->device.get();
    init_info.QueueFamily = ctx->graphicsQueueFamilyIndex;
    init_info.Queue = ctx->graphicsQueue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = descriptorPool.get();
    init_info.Allocator = nullptr;
    init_info.MinImageCount = ctx->imageCount - 1;
    init_info.ImageCount = ctx->imageCount;
    init_info.CheckVkResultFn = check_vk_result; // TODO: check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, renderPass.get());

    std::vector<vk::UniqueCommandBuffer> commandBuffers = ctx->device->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(commandPool.get(), vk::CommandBufferLevel::ePrimary, 1)); // kadangi tik init duomenu perkelimui - reikalingas tik 1 buferis
    commandBuffers[0]->begin(vk::CommandBufferBeginInfo{});
       ImGui_ImplVulkan_CreateFontsTexture(commandBuffers[0].get());
    commandBuffers[0]->end();
    ctx->graphicsQueue.submit(vk::SubmitInfo{{},{},{},1,&commandBuffers[0].get()}, {});
    ctx->graphicsQueue.waitIdle();
}

void vImGUI::createCommandPool()
{
    commandPool = ctx->device->createCommandPoolUnique(vk::CommandPoolCreateInfo{ vk::CommandPoolCreateFlagBits::eResetCommandBuffer, static_cast<uint32_t>(ctx->graphicsQueueFamilyIndex) });
}

void vImGUI::createCommandBuffers() {
    commandBuffers = ctx->device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool.get(), vk::CommandBufferLevel::ePrimary, ctx->imageCount));
}

void vImGUI::createRenderPass()
{
    auto colorAttachment = vk::AttachmentDescription{ {}, ctx->format, vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore, {}, {}, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR };

    auto colourAttachmentRef = vk::AttachmentReference{ 0, vk::ImageLayout::eColorAttachmentOptimal }; //vk::ImageLayout::eColorAttachmentOptimal

    auto subpass = vk::SubpassDescription{ {}, vk::PipelineBindPoint::eGraphics,
        /*inAttachmentCount*/ 0, nullptr, 1, &colourAttachmentRef };

    auto subpassDependency = vk::SubpassDependency{ VK_SUBPASS_EXTERNAL, 0, //Before drawing our GUI, we want our geometry to be already rendered. That means we want the pixels to be already written to the framebuffer. Fortunately, there is a stage called VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT for that and we can set our srcStageMask to it. We can also set our dstStageMask to this same value because our GUI will also be drawn to the same target. We’re basically waiting for pixels to be written before we can write pixels ourselves.
        vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
        {}, vk::AccessFlagBits::eColorAttachmentWrite };

    renderPass = ctx->device->createRenderPassUnique(vk::RenderPassCreateInfo{ {}, 1, &colorAttachment, 1, &subpass, 1, &subpassDependency });
}

void vImGUI::createFrameBuffers()
{
    frameBuffers = std::vector<vk::UniqueFramebuffer>(ctx->imageCount);

    for (size_t i = 0; i < ctx->imageCount; i++) {
        frameBuffers[i] = ctx->device->createFramebufferUnique(vk::FramebufferCreateInfo{
            {}, renderPass.get(), 1, &ctx->imageViews[i].get(), ctx->extent.width, ctx->extent.height, 1 });
    }
}

//void vImGUI::createPipline()
//{
//    auto rasterizer = vk::PipelineRasterizationStateCreateInfo{ {}, /*depthClamp*/ false,
//            /*rasterizeDiscard*/ false, vk::PolygonMode::eFill, /*cullmode*/ vk::CullModeFlagBits::eBack, //jei nieko nepaiso tai culling mode turi buti eBack/eFront
//        /*frontFace*/ vk::FrontFace::eCounterClockwise, {}, {}, {}, {}, 1.0f };

//    auto multisampling = vk::PipelineMultisampleStateCreateInfo{ {}, vk::SampleCountFlagBits::e1, false, 1.0 };

//    auto colorBlendAttachment = vk::PipelineColorBlendAttachmentState{ {}, /*srcCol*/ vk::BlendFactor::eOne,
//        /*dstCol*/ vk::BlendFactor::eZero, /*colBlend*/ vk::BlendOp::eAdd,
//        /*srcAlpha*/ vk::BlendFactor::eOne, /*dstAlpha*/ vk::BlendFactor::eZero,
//        /*alphaBlend*/ vk::BlendOp::eAdd,
//        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
//            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };

//    auto colorBlending = vk::PipelineColorBlendStateCreateInfo{ {}, /*logicOpEnable=*/false,
//        vk::LogicOp::eCopy, /*attachmentCount=*/1, /*colourAttachments=*/&colorBlendAttachment };

//    auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo{};

//    auto pipelineLayout = device->createPipelineLayoutUnique(pipelineLayoutInfo, nullptr);

//    auto colorAttachment = vk::AttachmentDescription{ {}, ctx->format, vk::SampleCountFlagBits::e1,
//        vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, {}, {}, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal};//vk::ImageLayout::eColorAttachmentOptimal }; //kai imgui VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, vk::ImageLayout::ePresentSrcKHR

//    auto colourAttachmentRef = vk::AttachmentReference{ 0, vk::ImageLayout::eColorAttachmentOptimal}; // orig vk::ImageLayout::eColorAttachmentOptimal

//    auto subpass = vk::SubpassDescription{ {}, vk::PipelineBindPoint::eGraphics,
//        /*inAttachmentCount*/ 0, nullptr, 1, &colourAttachmentRef };

////    auto subpassDependency = vk::SubpassDependency{ VK_SUBPASS_EXTERNAL, 0,
////        vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
////        {}, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite };

//    auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo{ {}, 0, {},
//    {}, {}, nullptr, &ctx->viewportState, &rasterizer, &multisampling,
//        nullptr, &colorBlending, nullptr, pipelineLayout.get(), renderPass.get(), 0 };

//    pipeline = device->createGraphicsPipelineUnique({}, pipelineCreateInfo).value;
//}

void vImGUI::updateCmdBuffers(int imageIndex)
{
    int i = imageIndex;
    commandBuffers[i].begin(vk::CommandBufferBeginInfo{});//vk::CommandBufferUsageFlagBits::eSimultaneousUse}); //TODO: fags?
        auto renderPassBeginInfo = vk::RenderPassBeginInfo{ renderPass.get(), frameBuffers[i].get(),
                vk::Rect2D{ { 0, 0 }, ctx->extent }};//, 1, &clv };
        commandBuffers[i].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[i]);
        commandBuffers[i].endRenderPass();
    commandBuffers[i].end();
}

void vImGUI::check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}
