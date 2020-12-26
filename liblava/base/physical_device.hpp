// file      : liblava/base/physical_device.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device.hpp>

namespace lava {

    struct physical_device : id_obj {
        using list = std::vector<physical_device>;
        using ref = physical_device const&;

        physical_device() = default;

        void initialize(VkPhysicalDevice vk_physical_device);

        bool supported(string_ref extension) const;
        bool get_queue_family(index& index, VkQueueFlags flags) const;

        device::create_param create_default_device_param() const;

        VkPhysicalDeviceProperties const& get_properties() const {
            return properties;
        }
        VkPhysicalDeviceFeatures const& get_features() const {
            return features;
        }
        VkPhysicalDeviceMemoryProperties const& get_memory_properties() const {
            return memory_properties;
        }

        VkQueueFamilyPropertiesList const& get_queue_family_properties() const {
            return queue_family_properties;
        }
        VkExtensionPropertiesList const& get_extension_properties() const {
            return extension_properties;
        }

        VkPhysicalDevice get() const {
            return vk_physical_device;
        }

        string get_device_type_string() const;

        bool swapchain_supported() const;
        bool surface_supported(index queue_family, VkSurfaceKHR surface) const;

    private:
        VkPhysicalDevice vk_physical_device = nullptr;

        VkPhysicalDeviceProperties properties = {};
        VkPhysicalDeviceFeatures features = {};
        VkPhysicalDeviceMemoryProperties memory_properties = {};

        VkQueueFamilyPropertiesList queue_family_properties;
        VkExtensionPropertiesList extension_properties;
    };

    using physical_device_ptr = physical_device;

} // namespace lava
