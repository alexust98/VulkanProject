#include "Window.hpp"


void Window::setupWindow(void)
{
    glfwInit();
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}


void Window::setupSurface(const VkInstance instance, VkAllocationCallbacks* pAllocator)
{
    if (glfwCreateWindowSurface(instance, window, pAllocator, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface!");
    }
}


void Window::destroyWindow(void)
{
    if (window != nullptr)
    {
        glfwDestroyWindow(window);
    }
}


void Window::destroySurface(const VkInstance instance, VkAllocationCallbacks* pAllocator)
{
    if (surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(instance, surface, pAllocator);
    }
}


const VkSurfaceKHR Window::getSurface(void) const
{
    return surface;
}
