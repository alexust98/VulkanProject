#ifndef VALLAYERS_HPP
#define VALLAYERS_HPP

#include "Config.hpp"

#include <vector>


class ValidationLayers
{
public:
    ValidationLayers() = default;
    ValidationLayers(const ValidationLayers&) =  delete;
    ValidationLayers& operator=(const ValidationLayers&) = delete;
    ValidationLayers(ValidationLayers&&) = delete;
    ValidationLayers& operator=(ValidationLayers&&) = delete;
    
    static const stringVector validationLayers;
    
    bool checkValidationLayerSupport(void);
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
       VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
       VkDebugUtilsMessageTypeFlagsEXT messageType,
       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
       void* pUserData);
    
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    
    void setupDebugMessenger(const VkInstance instance, const VkAllocationCallbacks* pAllocator = nullptr);
    void destroyDebugMessenger(const VkInstance instance, const VkAllocationCallbacks* pAllocator = nullptr);
    
private:
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    
    VkResult CreateDebugUtilsMessengerEXT(const VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator);
    
    void DestroyDebugUtilsMessengerEXT(const VkInstance instance, const VkAllocationCallbacks* pAllocator);
};

#endif
