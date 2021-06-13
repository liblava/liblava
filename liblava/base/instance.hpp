/**
 * @file         liblava/base/instance.hpp
 * @brief        Vulkan instance
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/physical_device.hpp>

namespace lava {

/**
 * @brief Vulkan instance information
 */
struct instance_info {
    /// Reference to a instance
    using ref = instance_info const&;

    /// Name of application
    name app_name = _lava_;

    /// Name of engine
    name engine_name = _liblava_;

    /// Version of application
    internal_version app_version;

    /// Version of engine
    internal_version engine_version;

    /// Required Vulkan API version
    api_version req_api_version = api_version::v1_0;
};

/**
 * @brief Vulkan instance
 */
struct instance : no_copy_no_move {
    /**
     * @brief Instance create parameters
     */
    struct create_param {
        /// Reference to instance create parameters
        using ref = create_param const&;

        /// List of layers to enable
        names layers{};

        /// List of extensions to enable
        names extensions{};
    };

    /**
     * @brief Debug configuration
     */
    struct debug_config {
        /// Reference to debug configuration
        using ref = debug_config const&;

        /// Validation
        bool validation = false;

        /// Renderdoc
        bool render_doc = false;

        /// Verbose
        bool verbose = false;

        /// Debug utils
        bool utils = false;
    };

    /**
     * @brief Instance singeton 
     * 
     * @return instance&    Instance
     */
    static instance& singleton() {
        static instance instance;
        return instance;
    }

    /**
     * @brief Create a new instance
     * 
     * @param param     Create parameters
     * @param debug     Debug configuration
     * @param info      Instance information
     * 
     * @return true     Create was successful
     * @return false    Create failed
     */
    bool create(create_param& param, debug_config::ref debug, instance_info::ref info);

    /**
     * @brief Destroy the instance
     */
    void destroy();

    /**
     * @brief Enumerate enabled layer properties
     * 
     * @return VkLayerPropertiesList    List of layer properties
     */
    static VkLayerPropertiesList enumerate_layer_properties();

    /**
     * @brief Enumerate enabled extension properties
     * 
     * @param layer_name                    Name of layer
     * 
     * @return VkExtensionPropertiesList    List of extension properties
     */
    static VkExtensionPropertiesList enumerate_extension_properties(name layer_name = nullptr);

    /**
     * @brief Get the physical devices
     * 
     * @return physical_device::list const&    List of physical devices
     */
    physical_device::list const& get_physical_devices() const {
        return physical_devices;
    }

    /**
     * @brief Get the first physical device
     * 
     * @return physical_device::ref    Physcial device
     */
    static physical_device::ref get_first_physical_device() {
        return singleton().physical_devices.front();
    }

    /**
     * @brief Get the Vulkan instance
     * 
     * @return VkInstance    Vulkan instance
     */
    static VkInstance get() {
        return singleton().vk_instance;
    }

    /**
     * @brief Get the version
     * 
     * @return internal_version    Version
     */
    static internal_version get_version();

    /**
     * @brief Get the debug configuration
     * 
     * @return debug_config::ref    Debug configuration
     */
    debug_config::ref get_debug_config() const {
        return debug;
    }

    /**
     * @brief Get the instance information
     * 
     * @return instance_info::ref    Instance information
     */
    instance_info::ref get_info() const {
        return info;
    }

private:
    /**
     * @brief Construct a new instance
     */
    explicit instance() = default;

    /**
     * @brief Destroy the instance
     */
    ~instance();

    /**
     * @brief Check the debug configuration and create parameters
     * 
     * @param param     Create parameters
     * 
     * @return true     Check was successful
     * @return false    Check failed
     */
    bool check_debug(create_param& param) const;

    /**
     * @brief Enumerate all available physical devices
     * 
     * @return true     Enumerate was successful
     * @return false    Enumerate failed
     */
    bool enumerate_physical_devices();

    /**
     * @brief Create a validation report
     * 
     * @return true     Create was successful
     * @return false    Create failed
     */
    bool create_validation_report();

    /**
     * @brief Destroy the validation report
     */
    void destroy_validation_report();

    /// Vulkan instance
    VkInstance vk_instance = nullptr;

    /// List of all physical devices
    physical_device::list physical_devices;

    /// Debug configuration
    debug_config debug;

    /// Instance information
    instance_info info;

    /// Debug utils messenger
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
};

/**
 * @brief Check instance create parameters
 * 
 * @param param     Create parameters
 * 
 * @return true     Check was successful
 * @return false    Check failed
 */
bool check(instance::create_param::ref param);

} // namespace lava
