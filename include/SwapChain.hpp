#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include "Config.hpp"


struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct SwapChainConfig
{
    VkExtent2D extent;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
};


class SwapChain
{
public:
    SwapChain() = default;
    SwapChain(const SwapChain&) =  delete;
    SwapChain& operator=(const SwapChain&) = delete;
    SwapChain(SwapChain&&) = delete;
    SwapChain& operator=(SwapChain&&) = delete;
    
    static SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice device, const VkSurfaceKHR surface);
    
    void setupSwapChain(const VkPhysicalDevice physicalDevice, const VkDevice logicalDevice, GLFWwindow* window, const VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator = nullptr);
    void setupImageViews(const VkDevice device, std::vector<const VkAllocationCallbacks*> pAllocators = {nullptr});
    void destroySwapChain(const VkDevice device, const VkAllocationCallbacks* pAllocator = nullptr);
    void destroyImageViews(const VkDevice device, std::vector<const VkAllocationCallbacks*> pAllocators = {nullptr});
    
    const SwapChainConfig getSwapChainConfig(void) const;
    
private:
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    SwapChainConfig scConfig;
    
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
    
    void populateSwapChainCreateInfo(VkSwapchainCreateInfoKHR& createInfo, const SwapChainSupportDetails swapChainSupport, const uint32_t* queueFamilyIndices);
    void populateImageViewCreateInfo(VkImageViewCreateInfo& createInfo, const VkImage image);
};

#endif
