/**
 * @file         liblava/base/platform.hpp
 * @brief        Stage platform
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/base/device.hpp"

namespace lava {

/**
 * @brief Stage platform
 */
struct platform {
    /// Pointer to platform
    using ptr = platform*;

    /**
     * @brief Create a managed device from a physical device
     * @param physical_device    Index of physical device
     * @return device::s_ptr     Vulkan device
     */
    device::s_ptr create(index physical_device = 0);

    /**
     * @brief Create a managed device with create parameters
     * @param param             Create parameters
     * @return device::s_ptr    Vulkan device
     */
    device::s_ptr create(device::create_param::ref param);

    /**
     * @brief Create a managed device
     * @param physical_device    Physical device
     * @return device::s_ptr     Pointer to device
     */
    device::ptr create_device(index physical_device = 0);

    /**
     * @brief Get all devices
     * @return device::s_list const&    List of devices
     */
    device::s_list const& get_devices() const {
        return devices;
    }

    /**
     * @brief Wait for idle on all managed devices
     */
    void wait_idle();

    /**
     * @brief Remove device from platform
     * @param device_id    Id of device
     * @return Remove was successful or failed
     */
    bool remove(id::ref device_id);

    /**
     * @brief Clear all managed devices
     */
    void clear();

    /// Create parameter function
    using create_param_func = std::function<void(device::create_param&)>;

    /// Called on create to adjust the create parameters
    create_param_func on_create_param;

private:
    /// List of managed devices
    device::s_list devices;
};

} // namespace lava
