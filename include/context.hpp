#pragma once
#include "VulkanDynamic.hpp"
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
//#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#ifndef CONTEXT_HPP
#define CONTEXT_HPP
struct Context
{
    uint32_t width = 800; // TODO: ar reikia?
    uint32_t height = 600;

    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t imageCount; //framebuffer size

//    vk::DispatchLoaderDynamic dldi;
    vk::UniqueInstance instance;
    vk::PhysicalDevice physicalDevice;
    vk::UniqueDevice device;
//    std::vector<vk::UniqueFramebuffer> *frameBuffers;
//    int queueFamily;
    size_t graphicsQueueFamilyIndex;
    size_t transferQueueFamilyIndex;
    size_t computeQueueFamilyIndex;
    size_t presentQueueFamilyIndex;
    vk::Queue graphicsQueue;
    vk::Queue computeQueue;
    vk::Queue transferQueue;
    vk::Queue presentQueue;

    std::vector<vk::UniqueImageView> imageViews; //swap chain images

    vk::Format format;
    vk::Extent2D extent; //swap chain extent
    vk::Viewport viewport;
    vk::Rect2D scissor;
    vk::PipelineViewportStateCreateInfo viewportState;
    std::vector<vk::UniqueSemaphore> imageAvailableSemaphores;
    std::vector<vk::UniqueSemaphore> renderFinishedSemaphores;
    std::vector<vk::UniqueFence> inFlightFences;
//    vk::UniqueFence computeFence;
};
#endif // CONTEXT_HPP
