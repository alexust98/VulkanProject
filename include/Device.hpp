#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "Config.hpp"
#include "Queue.hpp"


class Device
{
public:
    Device() = default;
    Device(const Device&) =  delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;
    
    static const stringVector deviceExtensions;
    
    void setupDevices(const VkInstance instance, const VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator = nullptr);
    void destroyDevices(const VkAllocationCallbacks* pAllocator = nullptr);
    
    const VkDevice getLogicalDevice(void) const;
    const VkPhysicalDevice getPhysicalDevice(void) const;
    const QueueFamilyIndices getQIndices(void) const;
    
private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    QueueFamilyIndices qIndices;
    
    int rateDeviceSuitability(const VkPhysicalDevice device, const VkSurfaceKHR surface);
    bool isDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR surface);
    bool checkDeviceExtensionSupport(const VkPhysicalDevice device);
    
    void populateDeviceCreateInfo(VkDeviceCreateInfo& createInfo, const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos, const VkPhysicalDeviceFeatures& deviceFeatures, stringVector& extensions);
    
    void pickPhysicalDevice(const VkInstance instance, const VkSurfaceKHR surface);
    void createLogicalDevice(const VkAllocationCallbacks* pAllocator);
};

#endif
