#pragma once
//#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
//#include "VulkanDynamic.hpp"
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "/home/kest/Downloads/renderdoc_1.12/include/renderdoc_app.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <fstream>
#include <set>
#include <array>
#include <thread>

#include "vImGUI.hpp"
#include "context.hpp"
#include "MyShape.hpp"
#include "MyCompute.hpp"

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

class vApp
{   
private:
    RENDERDOC_API_1_1_2 *rdoc_api = NULL;
//    void *renderDocLibHandle = NULL; // for debug
//    vk::DynamicLoader dl;
//    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
    Context ctx;
//    vk::DebugUtilsMessengerEXT debugMessenger;
//    vk::UniqueDebugUtilsMessengerEXT
    vk::UniqueDebugUtilsMessengerEXT debugMessengerU;
    GLFWwindow *window;
    vk::UniqueSurfaceKHR surface;
    vk::UniqueSwapchainKHR swapChain;
    vk::UniqueRenderPass primaryRenderPass;
//    vk::SubpassDescription primarySubpass; // for secondary cmdBuf
    std::vector<vk::UniqueFramebuffer> framebuffers; //for primary renderPass

    vk::UniqueCommandPool commandPool;
    std::vector<vk::UniqueCommandBuffer> primaryCommandBuffers;

    //depth testing
    vk::Format depthFormat; //our format
    vk::UniqueImage depthImage;
    vk::UniqueDeviceMemory depthImageMemory;
    vk::UniqueImageView depthImageView;

    std::unique_ptr<vImGUI> imgui;
    std::unique_ptr<MyShape> shape;
    std::unique_ptr<MyCompute> compute;
    std::unique_ptr<std::thread> computeThread;

    double xpos, ypos;
    double xpos0, ypos0;
    bool mousePressed{};
    bool autoRotateFlag{false};

    // GLFW funcs
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    // Vulkan functions
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

    void init(); //GLFW and Vulkan: devices, swapChain
    void createViewPort();
    void createPrimaryRenderPass();
    void createFrameBuffers();
    void createPrimaryCommandBuffers();
    void recordPrimaryCommandBuffers();

//    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    void mainLoop();
public:
    vApp();
    void run();
};

uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice physicalDevice);
