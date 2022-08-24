#include "Device.hpp"
#include <vector>
#include <map>

bool QueueFamilyIndices::isComplete(void)
{
    return graphicsFamily.has_value();
}


QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    uint32_t idx = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = idx;
        }
        
        if (indices.isComplete())
        {
            break;
        }

        idx++;
    }
    
    return indices;
}


int Device::rateDeviceSuitability(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    
    int score = 0;

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;

    // geometryShader is not supported in Metal API
    if (!deviceFeatures.tessellationShader or !deviceFeatures.sampleRateShading)
    {
        return 0;
    }

    return score;
}


bool Device::isDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(device);

    return indices.isComplete();
}


void Device::populateDeviceQueueCreateInfo(VkDeviceQueueCreateInfo& createInfo, const QueueFamilyIndices& indices)
{
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    createInfo.queueFamilyIndex = indices.graphicsFamily.value();
    createInfo.queueCount = 1;
    float queuePriority = 1.0f;
    createInfo.pQueuePriorities = &queuePriority;
}


void Device::populateDeviceCreateInfo(VkDeviceCreateInfo& createInfo, VkDeviceQueueCreateInfo& queueCreateInfo, VkPhysicalDeviceFeatures& deviceFeatures, std::vector<const char*>& extensions)
{
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;
    
    if (MOLTEN_VK)
    {
        extensions.emplace_back("VK_KHR_portability_subset");
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
    }
    else
    {
        createInfo.enabledExtensionCount = 0;
    }
    
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers::validationLayers.size());
        createInfo.ppEnabledLayerNames = ValidationLayers::validationLayers.data();
    } else
    {
        createInfo.enabledLayerCount = 0;
    }
}


void Device::pickPhysicalDevice(VkInstance instance)
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
        int score = rateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    if (candidates.rbegin()->first > 0)
    {
        physicalDevice = candidates.rbegin()->second;
    } else
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
}


void Device::createLogicalDevice(VkQueue graphicsQueue, const VkAllocationCallbacks* pAllocator)
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    
    VkDeviceQueueCreateInfo queueCreateInfo{};
    populateDeviceQueueCreateInfo(queueCreateInfo, indices);
    
    VkDeviceCreateInfo createInfo{};
    VkPhysicalDeviceFeatures deviceFeatures{};
    std::vector<const char*> extensions{};
    populateDeviceCreateInfo(createInfo, queueCreateInfo, deviceFeatures, extensions);
    
    if (vkCreateDevice(physicalDevice, &createInfo, pAllocator, &logicalDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device!");
    }
    
    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
}


void Device::setupDevices(VkInstance instance, VkQueue graphicsQueue, const VkAllocationCallbacks* pAllocator)
{
    pickPhysicalDevice(instance);
    
    if (physicalDevice != VK_NULL_HANDLE)
    {
        createLogicalDevice(graphicsQueue, pAllocator);
    }
}


void Device::destroyDevices(const VkAllocationCallbacks* pAllocator)
{
    if (logicalDevice != VK_NULL_HANDLE)
    {
        vkDestroyDevice(logicalDevice, pAllocator);
    }
}
