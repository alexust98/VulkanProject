#include "Queue.hpp"

#include <set>


bool QueueFamilyIndices::isComplete(void)
{
    return graphicsFamily.has_value() && presentFamily.has_value();
}


QueueFamilyIndices Queue::findQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR surface)
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
        
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, surface, &presentSupport);
        
        if (presentSupport)
        {
            indices.presentFamily = idx;
        }
        
        if (indices.isComplete())
        {
            break;
        }

        idx++;
    }
    
    return indices;
}

void Queue::populateDeviceQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo>& createInfos, const QueueFamilyIndices& indices)
{
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createInfo.queueFamilyIndex = queueFamily;
        createInfo.queueCount = 1;
        createInfo.pQueuePriorities = &queuePriority;
        createInfos.push_back(createInfo);
    }
}


void Queue::setupQueues(const VkDevice logicalDevice, const QueueFamilyIndices& indices)
{
    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
}
