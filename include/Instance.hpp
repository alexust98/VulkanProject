#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include "Config.hpp"

namespace Instance
{
    stringVector getRequiredInstanceExtensions(void);
    
    void populateInstanceCreateInfo(VkInstanceCreateInfo& createInfo, VkApplicationInfo& appInfo, VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo, stringVector& extensions);
    
    void populateApplicationInfo(VkApplicationInfo& appInfo);
};

#endif
