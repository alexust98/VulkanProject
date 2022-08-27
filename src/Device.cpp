#include "Device.hpp"
#include "ValLayers.hpp"
#include "SwapChain.hpp"

#include <map>
#include <set>
#include <string>


const stringVector Device::deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


int Device::rateDeviceSuitability(const VkPhysicalDevice device, const VkSurfaceKHR surface)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    
    
    if (!isDeviceSuitable(device, surface))
    {
        return 0;
    }
    
    // geometryShader is not supported in Metal API
    if (!deviceFeatures.tessellationShader or !deviceFeatures.sampleRateShading)
    {
        return 0;
    }
    
    int score = 0;

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}


bool Device::isDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR surface)
{
    QueueFamilyIndices indices = Queue::findQueueFamilies(device, surface);
    SwapChainSupportDetails swapChainSupport = SwapChain::querySwapChainSupport(device, surface);
    
    bool isSuitable = true;
    isSuitable &= indices.isComplete();
    isSuitable &= checkDeviceExtensionSupport(device);
    isSuitable &= !swapChainSupport.surfaceFormats.empty();
    isSuitable &= !swapChainSupport.presentModes.empty();
    
    return isSuitable;
}


bool Device::checkDeviceExtensionSupport(const VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
    
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}


void Device::populateDeviceCreateInfo(VkDeviceCreateInfo& createInfo, const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos, const VkPhysicalDeviceFeatures& deviceFeatures, stringVector& extensions)
{
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    
    if (MOLTEN_VK)
    {
        extensions.emplace_back("VK_KHR_portability_subset");
    }
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers::validationLayers.size());
        createInfo.ppEnabledLayerNames = ValidationLayers::validationLayers.data();
    } else
    {
        createInfo.enabledLayerCount = 0;
    }
}


void Device::pickPhysicalDevice(const VkInstance instance, const VkSurfaceKHR surface)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices)
    {
        int score = rateDeviceSuitability(device, surface);
        candidates.insert(std::make_pair(score, device));
    }

    if (candidates.rbegin()->first > 0)
    {
        physicalDevice = candidates.rbegin()->second;
        qIndices = Queue::findQueueFamilies(physicalDevice, surface);
    } else
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
}


void Device::createLogicalDevice(const VkAllocationCallbacks* pAllocator)
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    Queue::populateDeviceQueueCreateInfo(queueCreateInfos, qIndices);
    
    VkDeviceCreateInfo createInfo{};
    VkPhysicalDeviceFeatures deviceFeatures{};
    stringVector extensions{deviceExtensions};
    populateDeviceCreateInfo(createInfo, queueCreateInfos, deviceFeatures, extensions);
    
    if (vkCreateDevice(physicalDevice, &createInfo, pAllocator, &logicalDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device!");
    }
}


void Device::setupDevices(const VkInstance instance, const VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator)
{
    pickPhysicalDevice(instance, surface);
    
    if (physicalDevice != VK_NULL_HANDLE)
    {
        createLogicalDevice(pAllocator);
    }
}


void Device::destroyDevices(const VkAllocationCallbacks* pAllocator)
{
    if (logicalDevice != VK_NULL_HANDLE)
    {
        vkDestroyDevice(logicalDevice, pAllocator);
    }
}


const VkDevice Device::getLogicalDevice(void) const
{
    return logicalDevice;
}


const VkPhysicalDevice Device::getPhysicalDevice(void) const
{
    return physicalDevice;
}


const QueueFamilyIndices Device::getQIndices(void) const
{
    return qIndices;
}

