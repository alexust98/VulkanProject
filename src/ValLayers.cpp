#include "ValLayers.hpp"

#include <iostream>
#include <set>
#include <string>

const stringVector ValidationLayers::validationLayers = {"VK_LAYER_KHRONOS_validation"};


bool ValidationLayers::checkValidationLayerSupport(void)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    std::set<std::string> requiredLayers(validationLayers.begin(), validationLayers.end());

    for (const auto& layer : availableLayers)
    {
        requiredLayers.erase(layer.layerName);
    }

    return requiredLayers.empty();
}


VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayers::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    }
    
    return VK_FALSE;
}


VkResult ValidationLayers::CreateDebugUtilsMessengerEXT(const VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, &debugMessenger);
    } else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}


void ValidationLayers::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
}


void ValidationLayers::DestroyDebugUtilsMessengerEXT(const VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}


void ValidationLayers::setupDebugMessenger(const VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    if (!enableValidationLayers) return;
    
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);
    
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, pAllocator) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to set up debug messenger!");
    }

}

void ValidationLayers::destroyDebugMessenger(const VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    if (!enableValidationLayers) return;
    
    DestroyDebugUtilsMessengerEXT(instance, pAllocator);
}
