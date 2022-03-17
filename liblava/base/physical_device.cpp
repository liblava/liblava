/**
 * @file         liblava/base/physical_device.cpp
 * @brief        Vulkan physical device
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/base/instance.hpp>
#include <liblava/base/physical_device.hpp>

namespace lava {

//-----------------------------------------------------------------------------
physical_device::physical_device(VkPhysicalDevice vk_physical_device) {
    initialize(vk_physical_device);
}

//-----------------------------------------------------------------------------
void physical_device::initialize(VkPhysicalDevice pd) {
    vk_physical_device = pd;

    vkGetPhysicalDeviceProperties(vk_physical_device, &properties);
    vkGetPhysicalDeviceFeatures(vk_physical_device, &features);
    vkGetPhysicalDeviceMemoryProperties(vk_physical_device, &memory_properties);

    auto queue_family_count = 0u;
    vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &queue_family_count, nullptr);
    if (queue_family_count > 0) {
        queue_family_properties.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &queue_family_count, queue_family_properties.data());
    }

    auto extension_count = 0u;
    vkEnumerateDeviceExtensionProperties(vk_physical_device, nullptr, &extension_count, nullptr);
    if (extension_count > 0) {
        extension_properties.resize(extension_count);
        vkEnumerateDeviceExtensionProperties(vk_physical_device, nullptr, &extension_count, extension_properties.data());
    }
}

//-----------------------------------------------------------------------------
bool physical_device::supported(string_ref extension) const {
    for (auto& extension_property : extension_properties) {
        if (string(extension_property.extensionName) == extension)
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
bool physical_device::get_queue_family(index& index, VkQueueFlags flags) const {
    for (size_t i = 0, e = queue_family_properties.size(); i != e; ++i) {
        if ((queue_family_properties[i].queueFlags & flags) == flags) {
            index = to_index(i);
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
device::create_param physical_device::create_default_device_param() const {
    device::create_param create_param;
    create_param.physical_device = this;
    create_param.add_swapchain_extension();
    create_param.set_default_queues();

    return create_param;
}

//-----------------------------------------------------------------------------
name physical_device::get_device_name() const {
    return properties.deviceName;
}

//-----------------------------------------------------------------------------
string physical_device::get_device_type_string() const {
    string result;
    switch (properties.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        result = "OTHER";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        result = "INTEGRATED_GPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        result = "DISCRETE_GPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        result = "VIRTUAL_GPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        result = "CPU";
        break;
    default:
        result = "UNKNOWN";
        break;
    }
    return result;
}

//-----------------------------------------------------------------------------
bool physical_device::swapchain_supported() const {
    return supported(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

//-----------------------------------------------------------------------------
bool physical_device::surface_supported(index queue_family, VkSurfaceKHR surface) const {
    auto res = VK_FALSE;
    if (failed(vkGetPhysicalDeviceSurfaceSupportKHR(vk_physical_device,
                                                    queue_family, surface, &res)))
        return false;

    return res == VK_TRUE;
}

//-----------------------------------------------------------------------------
bool physical_device::profile_supported(VpProfileProperties profile) const {
    auto res = VK_FALSE;
    if (failed(vpGetPhysicalDeviceProfileSupport(instance::singleton().get(),
                                                 vk_physical_device,
                                                 &profile, &res)))
        return false;

    return res == VK_TRUE;
}

} // namespace lava
