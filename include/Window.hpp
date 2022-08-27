#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Config.hpp"


class Window
{
public:
    Window() = default;
    Window(const Window&) =  delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;
    
    GLFWwindow* window = nullptr;
    
    void setupWindow(void);
    void setupSurface(const VkInstance instance, VkAllocationCallbacks* pAllocator = nullptr);
    
    void destroyWindow(void);
    void destroySurface(const VkInstance instance, VkAllocationCallbacks* pAllocator = nullptr);
    
    const VkSurfaceKHR getSurface(void) const;
    
private:
    VkSurfaceKHR surface = VK_NULL_HANDLE;
};

#endif

