#pragma once
//#include "VulkanDynamic.hpp"
#define GLFW_INCLUDE_VULKAN
//#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#ifndef VIMGUI_HPP
#define VIMGUI_HPP

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "context.hpp"

//imgui
class vImGUI{
    GLFWwindow *window;
//    vk::UniquePipeline pipeline;
    Context *ctx;

    vk::UniqueCommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;

    std::vector<vk::UniqueFramebuffer> frameBuffers;
    vk::UniqueRenderPass renderPass;

    vk::UniqueDescriptorPool descriptorPool;

    static void check_vk_result(VkResult err);
    void createCommandPool();
    void createCommandBuffers();
    void createRenderPass();
    void createFrameBuffers(); // for RenderPass
//    void createPipline();

public:
    vImGUI(GLFWwindow *window, Context *ctx);
    ~vImGUI();
    void updateCmdBuffers(int imageIndex);
    void init();
//    void renderFrame();

//    vk::Pipeline *getPipline() {return &pipeline.get();};
    vk::RenderPass *getRenderPass() {return &renderPass.get();};
    vk::CommandBuffer *getCommandBuffer(uint32_t i) {return &commandBuffers[i];};
};

#endif // VIMGUI_HPP
