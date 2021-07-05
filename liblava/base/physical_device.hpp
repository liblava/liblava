/**
 * @file         liblava/base/physical_device.hpp
 * @brief        Vulkan physical device
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/device.hpp>

namespace lava {

/**
 * @brief Vulkan physical device
 */
struct physical_device : entity {
    /// Shared pointer to physical device
    using ptr = std::shared_ptr<physical_device>;

    /// List of physical devices
    using list = std::vector<physical_device::ptr>;

    /// Reference to physical device
    using ref = physical_device const&;

    /**
     * @brief Construct a new physical device
     */
    physical_device() = default;

    /**
     * @brief Construct and initialize a new physical device
     * 
     * @param vk_physical_device    Vulkan physical device
     */
    physical_device(VkPhysicalDevice vk_physical_device);

    /**
     * @brief Initialize the physical device
     * 
     * @param vk_physical_device    Vulkan physical device
     */
    void initialize(VkPhysicalDevice vk_physical_device);

    /**
     * @brief Check if extension is supported
     * 
     * @param extension    Extension to check
     * 
     * @return true        Extension is supported
     * @return false       Extension is unsupported
     */
    bool supported(string_ref extension) const;

    /**
     * @brief Get the queue family
     * 
     * @param index     Returned index of queue family
     * @param flags     Queue flags that must be set
     * 
     * @return true     Found a queue family
     * @return false    No queue family found
     */
    bool get_queue_family(index& index, VkQueueFlags flags) const;

    /**
     * @brief Create default device parameters
     * 
     * @return device::create_param Device    create parameters
     */
    device::create_param create_default_device_param() const;

    /**
     * @brief Get the properties
     * 
     * @return VkPhysicalDeviceProperties const&    Physical device properties
     */
    VkPhysicalDeviceProperties const& get_properties() const {
        return properties;
    }

    /**
     * @brief Get the features
     * 
     * @return VkPhysicalDeviceFeatures const&    Physical device features
     */
    VkPhysicalDeviceFeatures const& get_features() const {
        return features;
    }

    /**
     * @brief Get the memory properties
     * 
     * @return VkPhysicalDeviceMemoryProperties const&    Physical device memory properties
     */
    VkPhysicalDeviceMemoryProperties const& get_memory_properties() const {
        return memory_properties;
    }

    /**
     * @brief Get the queue family properties
     * 
     * @return VkQueueFamilyPropertiesList const&    List of queue family properties
     */
    VkQueueFamilyPropertiesList const& get_queue_family_properties() const {
        return queue_family_properties;
    }

    /**
     * @brief Get the extension properties
     * 
     * @return VkExtensionPropertiesList const&    List of extension properties
     */
    VkExtensionPropertiesList const& get_extension_properties() const {
        return extension_properties;
    }

    /**
     * @brief Get the Vulkan physical device
     * 
     * @return VkPhysicalDevice    Vulkan physical device
     */
    VkPhysicalDevice get() const {
        return vk_physical_device;
    }

    /**
     * @brief Get the device type as string
     * 
     * @return string    String representation of device type
     */
    string get_device_type_string() const;

    /**
     * @brief Check if swapchain is supported
     * 
     * @return true     Swapchain is supported
     * @return false    Swapchain is unsupported
     */
    bool swapchain_supported() const;

    /**
     * @brief Check if surface is supported
     * 
     * @param queue_family    Index of queue family
     * @param surface         Vulkan surface
     * 
     * @return true           Surface is supported
     * @return false          Surface is unsupported
     */
    bool surface_supported(index queue_family, VkSurfaceKHR surface) const;

private:
    /// Vulkan physical device
    VkPhysicalDevice vk_physical_device = nullptr;

    /// Physical device properties
    VkPhysicalDeviceProperties properties = {};

    /// Physical device features
    VkPhysicalDeviceFeatures features = {};

    /// Physical device memory properties
    VkPhysicalDeviceMemoryProperties memory_properties = {};

    /// List of queue family properties
    VkQueueFamilyPropertiesList queue_family_properties;

    /// List of extension properties
    VkExtensionPropertiesList extension_properties;
};

} // namespace lava
