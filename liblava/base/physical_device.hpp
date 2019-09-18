// file      : liblava/base/physical_device.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device.hpp>

namespace lava {

struct physical_device {

    using list = std::vector<physical_device>;

    physical_device() = default;

    void initialize(VkPhysicalDevice vk_device);

    bool is_supported(string_ref extension) const;
    bool get_queue_family(ui32& index, VkQueueFlags flags) const;

    device::create_param create_default_device_param() const;

    VkPhysicalDeviceProperties const& get_properties() const { return properties; }
    VkPhysicalDeviceFeatures const& get_features() const { return features; }
    VkPhysicalDeviceMemoryProperties const& get_memory_properties() const { return memory_properties; }

    VkQueueFamilyPropertiesList const& getQueueFamilyProperties() const { return queue_family_properties; }
    VkExtensionPropertiesList const& getExtensionProperties() const { return extension_properties; }

    VkPhysicalDevice get() const { return vk_device; }

    string get_device_type_string() const;

    bool is_swapchain_supported() const;
    bool is_surface_supported(ui32 queue_family_index, VkSurfaceKHR surface) const;

private:
    VkPhysicalDevice vk_device = nullptr;

    VkPhysicalDeviceProperties properties = {};
    VkPhysicalDeviceFeatures features = {};
    VkPhysicalDeviceMemoryProperties memory_properties = {};

    VkQueueFamilyPropertiesList queue_family_properties;
    VkExtensionPropertiesList extension_properties;
};

} // lava
