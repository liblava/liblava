/**
 * @file         liblava/base/platform.hpp
 * @brief        Stage platform
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/device.hpp>

namespace lava {

/**
 * @brief Stage platform
 */
struct platform {
    /**
     * @brief Create a managed device from a physical device
     * @param physical_device    Index of physical device
     * @return device::ptr       Vulkan device
     */
    device::ptr create(index physical_device = 0);

    /**
     * @brief Create a managed device with create parameters
     * @param param           Create parameters
     * @return device::ptr    Vulkan device
     */
    device::ptr create(device::create_param::ref param);

    /**
     * @brief Create a managed device
     * @param physical_device    Physical device
     * @return device_p          Pointer to device
     */
    device_p create_device(index physical_device = 0);

    /**
     * @brief Get all devices
     * @return device::list const&    List of devices
     */
    device::list const& get_all() const {
        return list;
    }

    /**
     * @brief Wait for idle on all managed devices
     */
    void wait_idle();

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
    device::list list;
};

/// Platform type
using platform_t = platform;

} // namespace lava
