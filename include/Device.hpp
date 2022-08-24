#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "Config.hpp"
#include "ValLayers.hpp"
#include <optional>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    
    bool isComplete(void);
};


class Device
{
public:
    void setupDevices(VkInstance instance, VkQueue graphicsQueue, const VkAllocationCallbacks* pAllocator = nullptr);
    void destroyDevices(const VkAllocationCallbacks* pAllocator = nullptr);
    
private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    
    int rateDeviceSuitability(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    
    void populateDeviceQueueCreateInfo(VkDeviceQueueCreateInfo& createInfo, const QueueFamilyIndices& indices);
    void populateDeviceCreateInfo(VkDeviceCreateInfo& createInfo, VkDeviceQueueCreateInfo& queueCreateInfo, VkPhysicalDeviceFeatures& deviceFeatures, std::vector<const char*>& extensions);
    
    void pickPhysicalDevice(VkInstance instance);
    void createLogicalDevice(VkQueue graphicsQueue, const VkAllocationCallbacks* pAllocator);
};

#endif
