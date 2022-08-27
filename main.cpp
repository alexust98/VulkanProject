#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>

#include "Config.hpp"
#include "Window.hpp"
#include "ValLayers.hpp"
#include "Device.hpp"
#include "Queue.hpp"
#include "SwapChain.hpp"


std::vector<const char*> getRequiredInstanceExtensions(void)
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
    {
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    
    if (MOLTEN_VK)
    {
        extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        extensions.emplace_back("VK_KHR_get_physical_device_properties2");
    }

    return extensions;
}


void populateApplicationInfo(VkApplicationInfo& appInfo)
{
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
}


void populateInstanceCreateInfo(VkInstanceCreateInfo& createInfo, VkApplicationInfo& appInfo, VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo, std::vector<const char*>& extensions)
{
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    
    if (MOLTEN_VK)
    {
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }
    
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers::validationLayers.size());
        createInfo.ppEnabledLayerNames = ValidationLayers::validationLayers.data();
        
        ValidationLayers::populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }
}


class HelloTriangleApplication
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
    Queue queue;
    Device dvc;
    SwapChain swapchain;
    
    
    void createInstance(void)
    {
        if (enableValidationLayers && !VL.checkValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }
        
        VkApplicationInfo appInfo{};
        populateApplicationInfo(appInfo);
        
        VkInstanceCreateInfo createInfo{};
        
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        std::vector<const char*> extensions = getRequiredInstanceExtensions();
        
        populateInstanceCreateInfo(createInfo, appInfo, debugCreateInfo, extensions);
        
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create instance!");
        }
    }

    
    void initVulkan(void)
    {
        createInstance();
        VL.setupDebugMessenger(instance);
        window.setupSurface(instance);
        dvc.setupDevices(instance, window.getSurface());
        queue.setupQueues(dvc.getLogicalDevice(), dvc.getQIndices());
        swapchain.setupSwapChain(dvc.getPhysicalDevice(), dvc.getLogicalDevice(), window.window, window.getSurface());
        swapchain.setupImageViews(dvc.getLogicalDevice());
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
        swapchain.destroyImageViews(dvc.getLogicalDevice());
        swapchain.destroySwapChain(dvc.getLogicalDevice());
        dvc.destroyDevices();
        VL.destroyDebugMessenger(instance);
        window.destroySurface(instance);
        vkDestroyInstance(instance, nullptr);
        window.destroyWindow();
        glfwTerminate();
    }
};


int main(void)
{
    HelloTriangleApplication app;

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
