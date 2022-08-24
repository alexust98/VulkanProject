#ifndef VALLAYERS_HPP
#define VALLAYERS_HPP

#include "Config.hpp"
#include <vector>


class ValidationLayers
{
public:
    
    static const std::vector<const char*> validationLayers;
    
    bool checkValidationLayerSupport(void);
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
       VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
       VkDebugUtilsMessageTypeFlagsEXT messageType,
       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
       void* pUserData);
    
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    
    void setupDebugMessenger(VkInstance instance, const VkAllocationCallbacks* pAllocator = nullptr);
    void destroyDebugMessenger(VkInstance instance, const VkAllocationCallbacks* pAllocator = nullptr);
    
private:
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator);
    
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, const VkAllocationCallbacks* pAllocator);
};

#endif
