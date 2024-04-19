/**
 * @file         liblava/frame/swapchain.hpp
 * @brief        Swapchain
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/resource/image.hpp"

namespace lava {

/**
 * @brief Swaphchain
 */
struct swapchain : entity {
    /**
     * @brief Create a new swapchain
     * @param device        Vulkan device
     * @param surface       Vulkan surface
     * @param format        Surface format
     * @param size          Size of swapchain
     * @param v_sync        V-Sync enabled
     * @param triple_buffer VK_PRESENT_MODE_MAILBOX_KHR preferred over VK_PRESENT_MODE_IMMEDIATE_KHR
     * @return Create was successful or failed
     */
    bool create(device::ptr device,
                VkSurfaceKHR surface,
                VkSurfaceFormatKHR format,
                uv2 size,
                bool v_sync = false,
                bool triple_buffer = true);

    /**
     * @brief Destroy the swapchain
     */
    void destroy();

    /**
     * @brief Resize the swapchain
     * @param new_size    New size of swapchain
     * @return Resize was successful or failed
     */
    bool resize(uv2 new_size);

    /**
     * @brief Request a reload of the swapchain
     */
    void request_reload() {
        reload_request_active = true;
    }

    /**
     * @brief Check if reload of the swapchain is requested
     * @return Reload is requested or not
     */
    bool reload_request() const {
        return reload_request_active;
    }

    /**
     * @brief Get the device
     * @return device::ptr    Vulkan device
     */
    device::ptr get_device() {
        return device;
    }

    /**
     * @brief Get the size of the swapchain
     * @return uv2    Swapchain size
     */
    uv2 get_size() const {
        return size;
    }

    /**
     * @brief Get the format of the swapchain
     * @return VkFormat    Swapchain format
     */
    VkFormat get_format() const {
        return format.format;
    }

    /**
     * @brief Get the color space of the swapchain
     * @return VkColorSpaceKHR    Swapchain color space
     */
    VkColorSpaceKHR get_color_space() const {
        return format.colorSpace;
    }

    /**
     * @brief Get the swapchain
     * @return VkSwapchainKHR    Vulkan swapchain
     */
    VkSwapchainKHR get() const {
        return vk_swapchain;
    }

    /**
     * @brief Get the backbuffer count
     * @return ui32    Number of backbuffers
     */
    ui32 get_backbuffer_count() const {
        return to_ui32(backbuffers.size());
    }

    /**
     * @brief Get the backbuffers
     * @return image::list const&    List of backbuffer images
     */
    image::list const& get_backbuffers() const {
        return backbuffers;
    }

    /**
     * @brief Swapchain callback
     */
    struct callback {
        /// List of callbacks
        using list = std::vector<callback*>;

        /// Created function
        using created_func = std::function<bool()>;

        /// Called on swapchain created
        created_func on_created;

        /// Destroyed function
        using destroyed_func = std::function<void()>;

        /// Called on swapchain destroyed
        destroyed_func on_destroyed;
    };

    /**
     * @brief Add callback to swapchain
     * @param cb    Callback to add
     */
    void add_callback(callback* cb);

    /**
     * @brief Remove callback from swapchain
     * @param cb    Callback to remove
     */
    void remove_callback(callback* cb);

    /**
     * @brief Check if V-Sync is enabled
     * @return V-Sync is active or not
     */
    bool v_sync() const {
        return v_sync_active;
    }

    /**
     * @brief Check if VK_PRESENT_MODE_MAILBOX_KHR is preferred over VK_PRESENT_MODE_IMMEDIATE_KHR
     * @return VK_PRESENT_MODE_MAILBOX_KHR preferred over VK_PRESENT_MODE_IMMEDIATE_KHR or not
     */
    bool triple_buffer() const {
        return triple_buffer_active;
    }

    /**
     * @brief Check if surface is supported by queue family index
     * @param queue_family    Queue family index
     * @return Surface is supported by queue family or not
     */
    bool surface_supported(index queue_family) const;

private:
    /**
     * @brief Choose present mode
     * @param present_modes        List of present modes to choose
     * @return VkPresentModeKHR    Chosen present mode
     */
    VkPresentModeKHR choose_present_mode(VkPresentModeKHRs const& present_modes) const;

    /**
     * @brief Create a swapchain create infomation (helper)
     * @param present_modes                List of present modes
     * @return VkSwapchainCreateInfoKHR    Swapchain create information
     */
    VkSwapchainCreateInfoKHR create_info(VkPresentModeKHRs present_modes);

    /**
     * @brief Set up the swapchain
     * @return Setup was successful or failed
     */
    bool setup();

    /**
     * @brief Tear down the swapchain
     */
    void teardown();

    /**
     * @brief Destroy swapchain backbuffer views
     */
    void destroy_backbuffer_views();

    /// Vulkan device
    device::ptr device = nullptr;

    /// Vulkan surface
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    /// Surface format
    VkSurfaceFormatKHR format = {};

    /// Vulkan swapchain
    VkSwapchainKHR vk_swapchain = VK_NULL_HANDLE;

    /// List of backbuffer images
    image::list backbuffers;

    /// Size of swapchain
    uv2 size;

    /// Reload request active state
    bool reload_request_active = false;

    /// V-Sync active state
    bool v_sync_active = false;

    /// Prefer VK_PRESENT_MODE_MAILBOX_KHR over VK_PRESENT_MODE_IMMEDIATE_KHR state
    bool triple_buffer_active = true;

    /// List of swapchain callbacks
    callback::list callbacks;
};

} // namespace lava
