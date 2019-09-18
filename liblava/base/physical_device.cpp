// file      : liblava/base/physical_device.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/base/physical_device.hpp>

namespace lava {

void physical_device::initialize(VkPhysicalDevice vk_device_) {

    vk_device = vk_device_;

    vkGetPhysicalDeviceProperties(vk_device, &properties);
    vkGetPhysicalDeviceFeatures(vk_device, &features);
    vkGetPhysicalDeviceMemoryProperties(vk_device, &memory_properties);

    auto queue_family_count = 0u;
    vkGetPhysicalDeviceQueueFamilyProperties(vk_device, &queue_family_count, nullptr);
    if (queue_family_count > 0) {

        queue_family_properties.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(vk_device, &queue_family_count, queue_family_properties.data());
    }

    auto extension_count = 0u;
    vkEnumerateDeviceExtensionProperties(vk_device, nullptr, &extension_count, nullptr);
    if (extension_count > 0) {

        extension_properties.resize(extension_count);
        vkEnumerateDeviceExtensionProperties(vk_device, nullptr, &extension_count, extension_properties.data());
    }
}

bool physical_device::is_supported(string_ref extension) const {

    for (auto& extension_property : extension_properties) {

        if (string(extension_property.extensionName) == extension)
            return true;
    }

    return false;
}

bool physical_device::get_queue_family(ui32& index, VkQueueFlags flags) const {

    for (size_t i = 0, e = queue_family_properties.size(); i != e; ++i) {

        if (queue_family_properties[i].queueFlags & flags) {

            index = (ui32)i;
            return true;
        }
    }

    return false;
}

device::create_param physical_device::create_default_device_param() const {

    device::create_param create_param;
    create_param._physical_device = this;
    create_param.set_default_queues();

    return create_param;
}

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

bool physical_device::is_swapchain_supported() const {

    return is_supported(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

bool physical_device::is_surface_supported(ui32 queue_family_index, VkSurfaceKHR surface) const {

    VkBool32 res = VK_FALSE;
    if (failed(vkGetPhysicalDeviceSurfaceSupportKHR(vk_device, queue_family_index, surface, &res)))
        return false;

    return res == VK_TRUE;
}

} // lava
