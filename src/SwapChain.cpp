#include "SwapChain.hpp"
#include "Queue.hpp"

#include <limits>
#include <algorithm>


SwapChainSupportDetails SwapChain::querySwapChainSupport(const VkPhysicalDevice device, const VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.surfaceFormats.data());
    }
    
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }
    
    return details;
}


VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    return availableFormats[0];
}


VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}


VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    } else
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent =
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}


void SwapChain::populateSwapChainCreateInfo(VkSwapchainCreateInfoKHR& createInfo, const SwapChainSupportDetails swapChainSupport, const uint32_t* queueFamilyIndices)
{
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    createInfo.minImageCount = imageCount;

    if (queueFamilyIndices[0] != queueFamilyIndices[1])
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }
    
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
}


void SwapChain::setupSwapChain(const VkPhysicalDevice physicalDevice, const VkDevice logicalDevice, GLFWwindow* window, const VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator)
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

    scConfig.extent = chooseSwapExtent(swapChainSupport.capabilities, window);
    scConfig.surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.surfaceFormats);
    scConfig.presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    
    VkSwapchainCreateInfoKHR createInfo{};
    QueueFamilyIndices indices = Queue::findQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    populateSwapChainCreateInfo(createInfo, swapChainSupport, queueFamilyIndices);
    
    createInfo.surface = surface;
    createInfo.presentMode = scConfig.presentMode;
    createInfo.imageFormat = scConfig.surfaceFormat.format;
    createInfo.imageColorSpace = scConfig.surfaceFormat.colorSpace;
    createInfo.imageExtent = scConfig.extent;
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    
    if (vkCreateSwapchainKHR(logicalDevice, &createInfo, pAllocator, &swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain!");
    }
    
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());
}


void SwapChain::populateImageViewCreateInfo(VkImageViewCreateInfo& createInfo, const VkImage image)
{
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = scConfig.surfaceFormat.format;
    
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
}


void SwapChain::setupImageViews(const VkDevice device, std::vector<const VkAllocationCallbacks*> pAllocators)
{
    swapChainImageViews.resize(swapChainImages.size());
    if (pAllocators[0] == nullptr)
    {
        pAllocators.resize(swapChainImages.size());
        std::fill(pAllocators.begin(), pAllocators.end(), nullptr);
    }
    
    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        populateImageViewCreateInfo(createInfo, swapChainImages[i]);
        
        if (vkCreateImageView(device, &createInfo, pAllocators[i], &swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image views!");
        }
    }
}


void SwapChain::destroySwapChain(const VkDevice device, const VkAllocationCallbacks* pAllocator)
{
    if (swapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(device, swapChain, pAllocator);
    }
}


void SwapChain::destroyImageViews(const VkDevice device, std::vector<const VkAllocationCallbacks*> pAllocators)
{
    if (pAllocators[0] == nullptr)
    {
        pAllocators.resize(swapChainImages.size());
        std::fill(pAllocators.begin(), pAllocators.end(), nullptr);
    }
    
    for (size_t i = 0; i < swapChainImageViews.size(); i++)
    {
        VkImageView imageView = swapChainImageViews[i];
        if (imageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(device, imageView, pAllocators[i]);
        }
    }
}


const SwapChainConfig SwapChain::getSwapChainConfig(void) const
{
    return scConfig;
}
