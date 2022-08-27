#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "Config.hpp"

#include <optional>


struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    
    bool isComplete(void);
};


class Queue
{
public:
    Queue() = default;
    Queue(const Queue&) =  delete;
    Queue& operator=(const Queue&) = delete;
    Queue(Queue&&) = delete;
    Queue& operator=(Queue&&) = delete;
    
    static QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR surface);
    static void populateDeviceQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo>& createInfos, const QueueFamilyIndices& indices);
    
    void setupQueues(const VkDevice logicalDevice, const QueueFamilyIndices& indices);
    
private:
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
};

#endif
