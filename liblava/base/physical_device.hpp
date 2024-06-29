/**
 * @file         liblava/base/physical_device.hpp
 * @brief        Vulkan physical device
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/base/device.hpp"

namespace lava {

/**
 * @brief Vulkan physical device
 */
struct physical_device : entity {
    /// Shared pointer to physical device
    using s_ptr = std::shared_ptr<physical_device>;

    /// List of physical devices
    using s_list = std::vector<s_ptr>;

    /// Reference to physical device
    using ref = physical_device const&;

    /**
     * @brief Make a new physical device
     * @param vk_physical_device    Vulkan physical device
     * @return s_ptr                Shared pointer to physical device
     */
    static s_ptr make(VkPhysicalDevice vk_physical_device) {
        return std::make_shared<physical_device>(vk_physical_device);
    }

    /**
     * @brief Construct a new physical device
     */
    physical_device() = default;

    /**
     * @brief Construct and initialize a new physical device
     * @param vk_physical_device    Vulkan physical device
     */
    physical_device(VkPhysicalDevice vk_physical_device);

    /**
     * @brief Initialize the physical device
     * @param vk_physical_device    Vulkan physical device
     */
    void initialize(VkPhysicalDevice vk_physical_device);

    /**
     * @brief Check if extension is supported
     * @param extension    Extension to check
     * @return Extension is supported or not
     */
    bool supported(string_ref extension) const;

    /**
     * @brief Get the queue family
     * @param index     Returned index of queue family
     * @param flags     Queue flags that must be set
     * @return Found a queue family or not
     */
    bool get_queue_family(index& index, VkQueueFlags flags) const;

    /**
     * @brief Create default device parameters
     * @return device::create_param Device    create parameters
     */
    device::create_param create_default_device_param() const;

    /**
     * @brief Get the properties
     * @return VkPhysicalDeviceProperties const&    Physical device properties
     */
    VkPhysicalDeviceProperties const& get_properties() const {
        return m_properties;
    }

    /**
     * @brief Get the features
     * @return VkPhysicalDeviceFeatures const&    Physical device features
     */
    VkPhysicalDeviceFeatures const& get_features() const {
        return m_features;
    }

    /**
     * @brief Get the memory properties
     * @return VkPhysicalDeviceMemoryProperties const&    Physical device memory properties
     */
    VkPhysicalDeviceMemoryProperties const& get_memory_properties() const {
        return m_memory_properties;
    }

    /**
     * @brief Get the queue family properties
     * @return VkQueueFamilyPropertiesList const&    List of queue family properties
     */
    VkQueueFamilyPropertiesList const& get_queue_family_properties() const {
        return m_queue_family_properties;
    }

    /**
     * @brief Get the extension properties
     * @return VkExtensionPropertiesList const&    List of extension properties
     */
    VkExtensionPropertiesList const& get_extension_properties() const {
        return m_extension_properties;
    }

    /**
     * @brief Get the Vulkan physical device
     * @return VkPhysicalDevice    Vulkan physical device
     */
    VkPhysicalDevice get() const {
        return m_vk_physical_device;
    }

    /**
     * @brief Get the device name
     * @return name    Name of device
     */
    name get_device_name() const;

    /**
     * @brief Get the device type as string
     * @return string    String representation of device type
     */
    string get_device_type_string() const;

    /**
     * @brief Get the driver version
     * @return sem_version    Driver version
     */
    sem_version get_driver_version() const;

    /**
     * @brief Check if swapchain is supported
     * @return Swapchain is supported or not
     */
    bool swapchain_supported() const;

    /**
     * @brief Check if surface is supported
     * @param queue_family    Index of queue family
     * @param surface         Vulkan surface
     * @return Surface is supported or not
     */
    bool surface_supported(index queue_family,
                           VkSurfaceKHR surface) const;

private:
    /// Vulkan physical device
    VkPhysicalDevice m_vk_physical_device = nullptr;

    /// Physical device properties
    VkPhysicalDeviceProperties m_properties = {};

    /// Physical device features
    VkPhysicalDeviceFeatures m_features = {};

    /// Physical device memory properties
    VkPhysicalDeviceMemoryProperties m_memory_properties = {};

    /// List of queue family properties
    VkQueueFamilyPropertiesList m_queue_family_properties;

    /// List of extension properties
    VkExtensionPropertiesList m_extension_properties;
};

} // namespace lava
