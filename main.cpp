#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>

#include "Config.hpp"
#include "Window.hpp"
#include "Instance.hpp"
#include "ValLayers.hpp"
#include "Device.hpp"
#include "Queue.hpp"
#include "SwapChain.hpp"
#include "Pipeline.hpp"


class VulkanProject
{
public:
    void run(void)
    {
        window.setupWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    Window window;
    VkInstance instance;
    ValidationLayers VL;
    Device device;
    Queue queue;
    SwapChain swapChain;
    VkRenderPass renderPass;
    Pipeline pipeline;
    
    void createInstance(void)
    {
        if (enableValidationLayers && !VL.checkValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }
        
        VkApplicationInfo appInfo{};
        Instance::populateApplicationInfo(appInfo);
        
        VkInstanceCreateInfo createInfo{};
        
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        stringVector extensions = Instance::getRequiredInstanceExtensions();
        
        Instance::populateInstanceCreateInfo(createInfo, appInfo, debugCreateInfo, extensions);
        
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create instance!");
        }
    }

    
    void initVulkan(void)
    {
        createInstance();
        VL.setupDebugMessenger(instance);
        window.setupSurface(instance);
        device.setupDevices(instance, window.getSurface());
        const VkDevice logicalDevice = device.getLogicalDevice();
        
        queue.setupQueues(logicalDevice, device.getQIndices());
        swapChain.setupSwapChain(device.getPhysicalDevice(), logicalDevice , window.window, window.getSurface());
        swapChain.setupImageViews(logicalDevice);
        createRenderPass();
        pipeline.setupGraphicsPipeline(logicalDevice, renderPass);
    }
    
    
    void createRenderPass(void)
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChain.getSwapChainConfig().surfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(device.getLogicalDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass!");
        }
    }

    
    void mainLoop(void)
    {
        while (!glfwWindowShouldClose(window.window))
        {
            glfwPollEvents();
        }
    }

    
    void cleanup(void)
    {
        const VkDevice logicalDevice = device.getLogicalDevice();
        
        pipeline.destroyGraphicsPipeline(logicalDevice);
        vkDestroyRenderPass(logicalDevice, renderPass, nullptr);
        swapChain.destroyImageViews(logicalDevice);
        swapChain.destroySwapChain(logicalDevice);
        device.destroyDevices();
        VL.destroyDebugMessenger(instance);
        window.destroySurface(instance);
        vkDestroyInstance(instance, nullptr);
        window.destroyWindow();
        glfwTerminate();
    }
};


int main(void)
{
    VulkanProject app;

    try
    {
        app.run();
    } catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
