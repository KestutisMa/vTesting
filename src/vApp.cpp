#include "vApp.hpp"

//VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

vApp::vApp()
{
//    vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
//    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    if(void *mod = dlopen("/home/kest/Downloads/renderdoc_1.12/lib/librenderdoc.so", RTLD_NOW | RTLD_NOLOAD))
    {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)dlsym(mod, "RENDERDOC_GetAPI");
        int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_4_1, (void **)&rdoc_api);
        assert(ret == 1);
    }
//         renderDocLibHandle = dlopen("/home/kest/Downloads/renderdoc_1.11/lib/librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);
//         if (!renderDocLibHandle)
//             std::cout << "cant open renderdoc lib\n";
//         else
//             std::cout << "renderdoc lib loaded\n";
}

void vApp::init() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(ctx.width, ctx.height, "vLBM-kest v1", nullptr, nullptr);

  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetWindowUserPointer(window, this);  // reikalingas callbackam

  vk::ApplicationInfo appInfo("vLBM", VK_MAKE_VERSION(1, 0, 0),
                              "No Engine", VK_MAKE_VERSION(1, 0, 0),
                              VK_API_VERSION_1_0);

  auto glfwExtensionCount = 0u;
  auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<const char *> extensionsVector(
      glfwExtensions, glfwExtensions + glfwExtensionCount);

 //******************
//********extensions

  extensionsVector.push_back("VK_EXT_debug_utils"); //instance extensions
//  extensionsVector.push_back("VK_KHR_ray_tracing");
//  extensionsVector.push_back("VK_KHR_shader_non_semantic_info");
//              VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME);
//  extensionsVector.push_back( VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
  const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME
//      VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
        };
  auto layers = std::vector<const char *>{"VK_LAYER_KHRONOS_validation"};

  //****************

//  vk::DynamicLoader dl;
  vk::DynamicLoader dl;
  PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
  VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

//  uint32_t count;
//  vk::enumerateInstanceExtensionProperties(nullptr, &count, nullptr);
//  std::vector<vk::ExtensionProperties> props(count);
//  vk::enumerateInstanceExtensionProperties(nullptr, &count, props.data());
//  for (auto e : props)
//      std::cout << e.extensionName << "\n";

  //  std::cout << VK_EXT_DEBUG_MARKER_EXTENSION_NAME << "\n";
  //    auto info = vk::DebugMarkerObjectNameInfoEXT{vk::DebugReportObjectTypeEXT::eCommandBuffer, (uint64_t)static_cast<VkCommandBuffer>(primaryCommandBuffers[0].get()), "labasidxStag"};
  //    ctx.device->debugMarkerSetObjectNameEXT(&info);

  if (enableValidationLayers)
      ctx.instance = vk::createInstanceUnique(
                  vk::InstanceCreateInfo{{},
                                         &appInfo,
                                         static_cast<uint32_t>(layers.size()),
                                         layers.data(),
                                         static_cast<uint32_t>(extensionsVector.size()),
                                         extensionsVector.data()});//, nullptr, VULKAN_HPP_DEFAULT_DISPATCHER);
  else
      ctx.instance = vk::createInstanceUnique(
                  vk::InstanceCreateInfo{{},
                                         &appInfo,
                                         0,
                                         0,
                                         static_cast<uint32_t>(extensionsVector.size()),
                                         extensionsVector.data()});//, nullptr, VULKAN_HPP_DEFAULT_DISPATCHER);

  VULKAN_HPP_DEFAULT_DISPATCHER.init(ctx.instance.get());

  debugMessengerU = ctx.instance->createDebugUtilsMessengerEXTUnique(
       vk::DebugUtilsMessengerCreateInfoEXT{
           {},
           vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
               | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
 //            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
 //            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
           ,
           vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
               vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
               vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
                  debugCallback});

//  debugMessengerU = instance->createDebugUtilsMessengerEXTUnique(
//       vk::DebugUtilsMessengerCreateInfoEXT{
//           {},
//           vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
//               | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
// //            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
// //            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
//           ,
//           vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
//               vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
//               vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
//                  debugCallback}, vk::Optional<const vk::AllocationCallbacks>{},vk::DispatchLoaderStatic{});

//  surface = vk::UniqueSurfaceKHR(*instance);
//  glfwCreateWindowSurface(*instance, window, nullptr, &surfaceTmp);
  VkSurfaceKHR surfaceTmp;
  //VkResult err =
    glfwCreateWindowSurface(ctx.instance.get(), window, nullptr, &surfaceTmp);
    surface = vk::UniqueSurfaceKHR(surfaceTmp, ctx.instance.get());
//  instance->destroySurfaceKHR(surfaceTmp);

  auto physicalDevices = ctx.instance->enumeratePhysicalDevices();

  ctx.physicalDevice = physicalDevices[1];
  for (auto d : physicalDevices) {
    (ctx.physicalDevice == d) ? printf("*") : printf(" ");
    std::cout << d.getProperties().deviceName << std::endl;
//    vk::PhysicalDeviceProperties props;
//    d.getProperties(&props);
//    std::cout << std::endl;
  }
  // auto physicalDevice =
  // physicalDevices[std::distance(physicalDevices.begin(),
  //     std::find_if(physicalDevices.begin(), physicalDevices.end(), [](const
  //     vk::PhysicalDevice& physicalDevice) {
  //         return strstr(physicalDevice.getProperties().deviceName, "Nvidia");
  //     }))];

  auto queueFamilyProperties = ctx.physicalDevice.getQueueFamilyProperties();

  ctx.graphicsQueueFamilyIndex = std::distance(
      queueFamilyProperties.begin(),
      std::find_if(queueFamilyProperties.begin(), queueFamilyProperties.end(),
                   [](vk::QueueFamilyProperties const &qfp) {
                     return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
                   }));

  ctx.computeQueueFamilyIndex = std::distance(
      queueFamilyProperties.begin(),
      std::find_if(queueFamilyProperties.begin(), queueFamilyProperties.end(),
                   [](vk::QueueFamilyProperties const &qfp) {
                     return qfp.queueFlags & vk::QueueFlagBits::eCompute;
                   }));

  ctx.transferQueueFamilyIndex = std::distance(
      queueFamilyProperties.begin(),
      std::find_if(queueFamilyProperties.begin(), queueFamilyProperties.end(),
                   [](vk::QueueFamilyProperties const &qfp) {
                     return qfp.queueFlags & vk::QueueFlagBits::eTransfer;
                   }));

  ctx.presentQueueFamilyIndex = 0u;
  for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
    if (ctx.physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i),
                                            surface.get())) {
      ctx.presentQueueFamilyIndex = i;
    }
  }

  std::set<size_t> uniqueQueueFamilyIndices = {ctx.graphicsQueueFamilyIndex, //QueueFamilyIndices must be unique in DeviceQueueCreateInfo
                                               ctx.computeQueueFamilyIndex,
                                               ctx.transferQueueFamilyIndex,
                                               ctx.presentQueueFamilyIndex};

  std::vector<uint32_t> FamilyIndices = {uniqueQueueFamilyIndices.begin(),
                                         uniqueQueueFamilyIndices.end()};

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

  float queuePriorities[] = {.9f, 0.1f, 0.9f, 0.9f};
  for (size_t queueFamilyIndex : uniqueQueueFamilyIndices) {
//      if (queueFamilyIndex == ctx.graphicsQueueFamilyIndex == ctx.computeQueueFamilyIndex) { //jei compute ir graphics queue sutampa (pvz. GTX960)
//          queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{
//                                         vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(queueFamilyIndex),
//                                         /*queueCount*/2, queuePriorities}); //TODO: queue priority
//      }
//      else
          queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{
                                         vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(queueFamilyIndex),
                                         /*queueCount*/4, queuePriorities}); //TODO: queue priority
  }

  // Create physical device
  auto phyDevFeatures = vk::PhysicalDeviceFeatures{};
  phyDevFeatures.samplerAnisotropy = VK_TRUE; //for textures
  ctx.device = ctx.physicalDevice.createDeviceUnique(vk::DeviceCreateInfo(
      vk::DeviceCreateFlags(), queueCreateInfos.size(), queueCreateInfos.data(),
                                                         0, nullptr, deviceExtensions.size(), deviceExtensions.data(), &phyDevFeatures));

  VULKAN_HPP_DEFAULT_DISPATCHER.init(ctx.device.get());
//auto dldi = vk::DispatchLoaderDynamic(ctx.instance.get(), vkGetInstanceProcAddr, ctx.device.get(), vkGetDeviceProcAddr);

// debugMessenger = ctx.instance->createDebugUtilsMessengerEXT(
//      vk::DebugUtilsMessengerCreateInfoEXT{
//          {},
//          vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
//              | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
////            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
////            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
//          ,
//          vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
//              vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
//              vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
//          debugCallback},
//      nullptr, dldi);

  // ******* Create Swapchain
  struct SM {
    vk::SharingMode sharingMode;
    uint32_t familyIndicesCount;
    uint32_t *familyIndicesDataPtr;
  } sharingModeUtil{
      (ctx.graphicsQueueFamilyIndex != ctx.presentQueueFamilyIndex)
          ? SM{vk::SharingMode::eConcurrent, 2u, FamilyIndices.data()}
          : SM{vk::SharingMode::eExclusive, 0u,
               static_cast<uint32_t *>(nullptr)}};

  // needed for validation warnings
  auto capabilities = ctx.physicalDevice.getSurfaceCapabilitiesKHR(*surface);
  auto formats = ctx.physicalDevice.getSurfaceFormatsKHR(*surface);

  ctx.format = vk::Format::eB8G8R8A8Unorm;
  ctx.extent = vk::Extent2D{ctx.width, ctx.height};

  ctx.imageCount = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount > 0 && ctx.imageCount > capabilities.maxImageCount) {
      ctx.imageCount = capabilities.maxImageCount;
  }

  vk::SwapchainCreateInfoKHR swapChainCreateInfo(
      {}, surface.get(), ctx.imageCount, ctx.format, vk::ColorSpaceKHR::eSrgbNonlinear,
      ctx.extent, 1, vk::ImageUsageFlagBits::eColorAttachment,
      sharingModeUtil.sharingMode, sharingModeUtil.familyIndicesCount,
      sharingModeUtil.familyIndicesDataPtr,
      vk::SurfaceTransformFlagBitsKHR::eIdentity,
      vk::CompositeAlphaFlagBitsKHR::eOpaque, vk::PresentModeKHR::eImmediate, true,
      nullptr);

  swapChain = ctx.device->createSwapchainKHRUnique(swapChainCreateInfo);

  std::vector<vk::Image> swapChainImages =
      ctx.device->getSwapchainImagesKHR(swapChain.get());

  ctx.imageViews.reserve(swapChainImages.size());
  for (auto image : swapChainImages) {
    vk::ImageViewCreateInfo imageViewCreateInfo(
        vk::ImageViewCreateFlags(), image, vk::ImageViewType::e2D, ctx.format,
        vk::ComponentMapping{vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
                             vk::ComponentSwizzle::eB,
                             vk::ComponentSwizzle::eA},
        vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    ctx.imageViews.push_back(ctx.device->createImageViewUnique(imageViewCreateInfo));
  }

  //
  //Depth-testing
  //

  auto formatCandidates = {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint};
  auto tiling = vk::ImageTiling::eOptimal;
  auto features = vk::FormatFeatureFlagBits::eDepthStencilAttachment;
  for (vk::Format f : formatCandidates) {
      vk::FormatProperties props;
      ctx.physicalDevice.getFormatProperties(f, &props);
      if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
          depthFormat = f;
      } else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
          depthFormat = f;
      }
  }
  depthImage = ctx.device->createImageUnique(vk::ImageCreateInfo{
        {}, vk::ImageType::e2D, depthFormat, vk::Extent3D{ctx.extent.width, ctx.extent.height, 1 }, 1, 1, vk::SampleCountFlagBits::e1, tiling, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::SharingMode::eExclusive, {}, vk::ImageLayout::eUndefined});
  vk::MemoryRequirements memRequirements;
  auto memoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
  ctx.device->getImageMemoryRequirements(depthImage.get(), &memRequirements);
  depthImageMemory = ctx.device->allocateMemoryUnique(vk::MemoryAllocateInfo{memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags, ctx.physicalDevice)});
  ctx.device->bindImageMemory(depthImage.get(), depthImageMemory.get(), 0);
  vk::ImageAspectFlags aspectFlags = {vk::ImageAspectFlagBits::eDepth};
  depthImageView = ctx.device->createImageViewUnique(vk::ImageViewCreateInfo{{}, depthImage.get(), vk::ImageViewType::e2D, depthFormat, {}, vk::ImageSubresourceRange{aspectFlags, 0, 1, 0, 1}});

  //
  //Sync-Objects
  //

  ctx.imageAvailableSemaphores.resize(ctx.MAX_FRAMES_IN_FLIGHT);
  ctx.renderFinishedSemaphores.resize(ctx.MAX_FRAMES_IN_FLIGHT);
  ctx.inFlightFences.resize(ctx.MAX_FRAMES_IN_FLIGHT);

  try {
      for (uint32_t i = 0; i < ctx.MAX_FRAMES_IN_FLIGHT; i++) {
          ctx.imageAvailableSemaphores[i] = ctx.device->createSemaphoreUnique({});
          ctx.renderFinishedSemaphores[i] = ctx.device->createSemaphoreUnique({});
          ctx.inFlightFences[i] = ctx.device->createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
      }
  } catch (vk::SystemError err) {
      throw std::runtime_error("failed to create synchronization objects for a frame!");
  }


  ctx.graphicsQueue = ctx.device->getQueue(ctx.graphicsQueueFamilyIndex, 0);
  ctx.computeQueue = ctx.device->getQueue(ctx.computeQueueFamilyIndex, 1);
  ctx.transferQueue = ctx.device->getQueue(ctx.transferQueueFamilyIndex, 2);
  ctx.presentQueue = ctx.device->getQueue(ctx.presentQueueFamilyIndex, 3);
}


void vApp::createViewPort()
{
 ctx.viewport =
        vk::Viewport{ 0.0f, 0.0f, static_cast<float>(ctx.width), static_cast<float>(ctx.height), 0.0f, 1.0f };

 ctx.scissor = vk::Rect2D{ { 0, 0 }, ctx.extent };

 ctx.viewportState = vk::PipelineViewportStateCreateInfo{ {}, 1, &ctx.viewport, 1, &ctx.scissor };
}

void vApp::createPrimaryRenderPass() //TODO
{

    auto colorAttachment = vk::AttachmentDescription{ {}, ctx.format, vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, {}, {}, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal};

    auto depthAttachment = vk::AttachmentDescription{ {}, depthFormat, vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    auto colourAttachmentRef = vk::AttachmentReference{ 0, vk::ImageLayout::eColorAttachmentOptimal}; // orig vk::ImageLayout::eColorAttachmentOptimal

    auto depthAttachmentRef = vk::AttachmentReference{ 1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    auto primarySubpass = vk::SubpassDescription{ {}, vk::PipelineBindPoint::eGraphics,
            /*inAttachmentCount*/ 0, nullptr, 1, &colourAttachmentRef, {}, &depthAttachmentRef };

    auto subpassDependency = vk::SubpassDependency{ VK_SUBPASS_EXTERNAL, 0,
        vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        {}, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite };

    primaryRenderPass = ctx.device->createRenderPassUnique(
                vk::RenderPassCreateInfo{ {}, attachments.size(), attachments.data(), 1, &primarySubpass, 1, &subpassDependency });
}

void vApp::createFrameBuffers()
{
    framebuffers = std::vector<vk::UniqueFramebuffer>(ctx.imageCount);

    for (size_t i = 0; i < ctx.imageCount; i++) {
        std::array<vk::ImageView, 2> attachments = {
            ctx.imageViews[i].get(),
            depthImageView.get()
        };
        framebuffers[i] = ctx.device->createFramebufferUnique(vk::FramebufferCreateInfo{
            {}, primaryRenderPass.get(), attachments.size(), attachments.data(), ctx.extent.width, ctx.extent.height, 1 });
    }
}

//FrameBuffers, CommandBuffers
void vApp::createPrimaryCommandBuffers() {
    commandPool = ctx.device->createCommandPoolUnique({ {}, static_cast<uint32_t>(ctx.graphicsQueueFamilyIndex) });
    primaryCommandBuffers = ctx.device->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(commandPool.get(), vk::CommandBufferLevel::ePrimary, ctx.imageCount));;
}

void vApp::recordPrimaryCommandBuffers() { //FIXME: TODO: record primary command buffer
//    auto aa = VULKAN_HPP_DEFAULT_DISPATCHER;
//    auto info = vk::DebugMarkerObjectNameInfoEXT{vk::DebugReportObjectTypeEXT::eCommandBuffer, (uint64_t)static_cast<VkCommandBuffer>(primaryCommandBuffers[0].get()), "labasidxStag"};
//    ctx.device->debugMarkerSetObjectNameEXT(&info);
    for (size_t i = 0; i < primaryCommandBuffers.size(); i++) {

        //TODO:
        primaryCommandBuffers[i]->begin(vk::CommandBufferBeginInfo{});
            vk::ClearValue colorClearValue{ vk::ClearColorValue{ std::array<float,4>{0.0f,.2f,0,1.f} } };
            vk::ClearValue depthClearValue{ vk::ClearDepthStencilValue{ /*depth*/ 1.0f, 0} };
            std::array<vk::ClearValue, 2> clearValues{colorClearValue, depthClearValue};
            auto renderPassBeginInfo = vk::RenderPassBeginInfo{ primaryRenderPass.get(), framebuffers[i].get(),
                vk::Rect2D{ { 0, 0 }, ctx.extent }, clearValues.size(), clearValues.data() };
//            vk::DeviceSize offsets[] = { 0 };
            primaryCommandBuffers[i]->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eSecondaryCommandBuffers);//eSecondaryCommandBuffers); //TODO: eInline?
//                  primaryCommandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, shape->pipeline.get());
                  primaryCommandBuffers[i]->executeCommands(1u,shape->getSecondaryCommandBuffer(i));
//                primaryCommandBuffers[i]->bindVertexBuffers(0,1, &vertexBuffer.get(), offsets );
//                primaryCommandBuffers[i]->bindIndexBuffer(vertexIndexBuffer.get(),0,vk::IndexType::eUint16);
//                primaryCommandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());
//                primaryCommandBuffers[i]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, ctx.pipelineLayout.get(), 0, 1, &descriptorSets[i].get(), 0, {});
//                primaryCommandBuffers[i]->drawIndexed(static_cast<uint32_t>(vertexIndices.size()), 2, 0, 0, 0);
//                  primaryCommandBuffers[i]->draw(3, 1, 0, 0);
            primaryCommandBuffers[i]->endRenderPass();

        primaryCommandBuffers[i]->end();
    }
}


 //TODO:
void vApp::run() 
{
    init();//GLFW and Vulkan
    createViewPort();
    createPrimaryRenderPass();
    createFrameBuffers();
    createPrimaryCommandBuffers();
    imgui = std::unique_ptr<vImGUI>( new vImGUI(window, &ctx));

    glm::vec3 gridSize = {2,2,1};
    std::vector<StorageBufferStruct> storageBufferHostData; //for compute
    std::vector<Vertex> vertices; //for vertex
    std::vector<uint32_t> vertexIndices;
    std::srand(1);

    vertices.push_back({ { 0.0, 0, 0, 0 }, {0.0f, 1.0f, 0, 0}, {1.0f, 0.0f} });
    vertices.push_back({ { 1.0, 0, 0, 0 }, {0.0f, 1.0f, 0, 0}, {0.0f, 0.0f} });
    vertices.push_back({ { 0.0, 1.0, 0, 0 }, {0.0f, 1.0f, 0, 0}, {0.0f, 1.0f} });
    vertices.push_back({ { 1.0, 1.0, 0, 0 }, {0.0f, 1.0f, 0, 0}, {1.0f, 1.0f} });
    vertexIndices.push_back(0);
    vertexIndices.push_back(1);
    vertexIndices.push_back(2);

    vertexIndices.push_back(2);
    vertexIndices.push_back(3);
    vertexIndices.push_back(1);

    struct alignas(16) StorageBufferStruct { //shader storage buffer, (visi glsl vec tipai 16bytes aligned)
//        float fin[9] = {};
        float   rho = {}; // kintamasis negali prasideti vienoje 16 bytes puseje, o pasibaigti kitoje
//        glm::vec2   u = {};
//        float fout[9] = {};
    } a;
    storageBufferHostData.push_back( {1.0} );
//    storageBufferHostData.push_back( {1.0} );
//    storageBufferHostData.push_back( {1.0} );
//    storageBufferHostData.push_back( {1.0} );

    // TODO: test many vertices
//    int sizeX = 4+1;
//    int sizeY = 4+1;
//    for (int x = 0; x < sizeX; x++) {
//        for (int y = 0; y < sizeY; y++) {
//            vertices.push_back({ { static_cast<float>(x) / sizeX, static_cast<float>(y) / sizeY, 0, 0 }, {0.0f, 1.0f, 0, 0} });
//        }
//    }
//    for (int x = 0; x < sizeX-1; x++) {
//        for (int y = 0; y < sizeY-1; y++) {
//            vertexIndices.push_back(y*sizeX + x);
//            vertexIndices.push_back((y+1)*sizeX + x);
//            vertexIndices.push_back((y+1)*sizeX + x +1);
//        }
//    }

    compute = std::unique_ptr<MyCompute>( new MyCompute(&ctx, storageBufferHostData));
    shape = std::unique_ptr<MyShape>( new MyShape(&ctx, vk::CommandBufferInheritanceInfo{primaryRenderPass.get()}, vertices, vertexIndices, compute->getStorageBuffer(), gridSize));//, &framebuffers);//, &primarySubpass);

    recordPrimaryCommandBuffers();

//    computeThread = std::unique_ptr<std::thread> (new std::thread([this](){
//        printf("\ncompThreadFunc start...\n");
//        while (!glfwWindowShouldClose(window))
//        {
////                    if(rdoc_api) rdoc_api->StartFrameCapture(NULL, NULL);
            compute->runComputeCommandBuffer();
////                    if(rdoc_api) rdoc_api->EndFrameCapture(NULL, NULL);
//            // printf("\ncompThreadFunc run\n");
//            std::this_thread::sleep_for(std::chrono::duration(std::chrono::microseconds(2000)));
//        }
//        printf("\ncompThreadFunc done...\n");
//    }));

//    std::thread compThread2([&](){
//        printf("\ncompThreadFunc2 start...\n");
//        while (!glfwWindowShouldClose(window))
//        {
////            glfwPollEvents();




////            std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(20)));
//        }
//        printf("\ncompThreadFunc2 done...\n");
//    });


//            for (int i = 0; i < 3; i++)
//                imgui->updateCmdBuffers(i);
    size_t currentFrame = 0;
    glm::vec3 rotationAngles = {0,0,0};
    float fov = 45.f;
//    compute->runComputeCommandBuffer();
    while (!glfwWindowShouldClose(window))
    {
        std::cout << std::flush; // kad spausdintu be pauziu
//        if(rdoc_api) rdoc_api->StartFrameCapture(NULL, NULL);
//        if(rdoc_api) rdoc_api->EndFrameCapture(NULL, NULL);

//        compute->runComputeCommandBuffer();

        std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(15)));

        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // bool ee = false;
        // ImGui::SetNextWindowPos(ImVec2(10, 20), ImGuiCond_FirstUseEver);
        // ImGui::SetNextWindowSize(ImVec2(50, 10), ImGuiCond_FirstUseEver);
//        ImGui::ShowMetricsWindow();
        ImGui::Begin("wind1");
        ImGui::SliderFloat("x",&rotationAngles.x,0,360.f);
        ImGui::SliderFloat("y",&rotationAngles.y,0,360.f);
        ImGui::SliderFloat("z",&rotationAngles.z,0,360.f);
        ImGui::SliderFloat("fov",&fov,0,180.f);
        if (mousePressed)
            ImGui::Text("%f %f", xpos-xpos0, ypos-ypos0);
        ImGui::Checkbox("rotate", &autoRotateFlag);
        // if (ImGui::Button("Save"))

    //    ImGui::InputText("stringaaaa", buf, IM_ARRAYSIZE(buf));
        // float a;
        // ImGui::SliderFloat("float", &a, 0.0f, 1.0f);
//        ImGui::ShowDemoWindow();
        ImGui::End();
        ImGui::Render();

        ctx.device->waitForFences(1, &ctx.inFlightFences[currentFrame].get(), VK_TRUE, std::numeric_limits<uint64_t>::max());
        ctx.device->resetFences(1, &ctx.inFlightFences[currentFrame].get());

        auto imageIndex = ctx.device->acquireNextImageKHR(swapChain.get(),std::numeric_limits<uint64_t>::max(), ctx.imageAvailableSemaphores[currentFrame].get(),{});

        shape->updateUniformBuffer(imageIndex.value, autoRotateFlag, rotationAngles, fov); //rotation

        // imgui
                imgui->updateCmdBuffers(imageIndex.value);
        // end imgui

        vk::PipelineStageFlags waitStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        std::array<vk::CommandBuffer, 2> submitCommandBuffers = {primaryCommandBuffers[imageIndex.value].get(), *imgui->getCommandBuffer(imageIndex.value)};  //imgui
        auto submitInfo = vk::SubmitInfo{
                1, &ctx.imageAvailableSemaphores[currentFrame].get(), &waitStageMask,
                2u, submitCommandBuffers.data(),
                1, &ctx.renderFinishedSemaphores[currentFrame].get() };

        ctx.graphicsQueue.submit(submitInfo, ctx.inFlightFences[currentFrame].get());

        auto presentInfo = vk::PresentInfoKHR{ 1, &ctx.renderFinishedSemaphores[currentFrame].get(),
                                               1, &swapChain.get(), &imageIndex.value };
        ctx.presentQueue.presentKHR(presentInfo);

        currentFrame = (currentFrame + 1) % ctx.MAX_FRAMES_IN_FLIGHT;

    }
//    computeThread->join();
//    StorageBufferStruct *stor = compute->getbackSSB();
//    printf("\nstoragebuff.test=%f",stor->pos[0].x);
    printf("\nBye0\n");
    ctx.device->waitIdle();
    printf("\nBye1\n");
//    delete imgui;
//    delete shape;
//    delete compute;
//    auto dldi = vk::DispatchLoaderDynamic(ctx.instance.get(), vkGetInstanceProcAddr);
//    ctx.instance->destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, VULKAN_HPP_DEFAULT_DISPATCHER);//ctx.dldi);
//    auto dldi = vk::DispatchLoaderDynamic(ctx.instance.get(), vkGetInstanceProcAddr, ctx.device.get(), vkGetDeviceProcAddr);
//    ctx.instance->destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
}


void vApp::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    printf("key pressed = %d\n", key);
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
//  if (key == GLFW_KEY_A && action == GLFW_PRESS) {
//      const std::vector<Vertex> vertices = {
//          {{0.0f, -0.5f}, {0.0f, 0.0f, 1.0f}},
//          {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
//          {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

//      vApp *obj = (vApp*)glfwGetWindowUserPointer(window);
//      auto size = sizeof(vertices[0]) * vertices.size();
////      void* data = obj->device->mapMemory(*obj->vertexBufferMemory, 0, size);
////      memcpy(data, vertices.data(), (size_t)size);
////      obj->device->unmapMemory(*obj->vertexBufferMemory);
//      void* data = obj->device->mapMemory(*obj->stagingVertexBufferMemory, 0, size);
//      memcpy(data, vertices.data(), (size_t)size);
//      obj->device->unmapMemory(*obj->stagingVertexBufferMemory);

//      obj->copyBuffer(obj->stagingVertexBuffer, obj->vertexBuffer, size);
//  }
}

void vApp::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        vApp *o = (vApp*)glfwGetWindowUserPointer(window);
        if (action == GLFW_PRESS) {
//            glfwGetCursorPos(window, &o->xpos0, &o->ypos0);
        }
        o->mousePressed = action == GLFW_PRESS ? true : false;
    }
}

void vApp::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    vApp *o = (vApp*)glfwGetWindowUserPointer(window);
    if (o->mousePressed) {
        o->xpos = xpos;
        o->ypos = ypos;
    }
    else {
        o->xpos0 = xpos;
        o->ypos0 = ypos;
    }

}

VKAPI_ATTR VkBool32 VKAPI_CALL
vApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                    void *pUserData) {
  std::cout << "labas:***************\n";
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice physicalDevice) {
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}
