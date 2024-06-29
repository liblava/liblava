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
        m_reload_request_active = true;
    }

    /**
     * @brief Check if reload of the swapchain is requested
     * @return Reload is requested or not
     */
    bool reload_request() const {
        return m_reload_request_active;
    }

    /**
     * @brief Get the device
     * @return device::ptr    Vulkan device
     */
    device::ptr get_device() {
        return m_device;
    }

    /**
     * @brief Get the size of the swapchain
     * @return uv2    Swapchain size
     */
    uv2 get_size() const {
        return m_size;
    }

    /**
     * @brief Get the format of the swapchain
     * @return VkFormat    Swapchain format
     */
    VkFormat get_format() const {
        return m_format.format;
    }

    /**
     * @brief Get the color space of the swapchain
     * @return VkColorSpaceKHR    Swapchain color space
     */
    VkColorSpaceKHR get_color_space() const {
        return m_format.colorSpace;
    }

    /**
     * @brief Get the swapchain
     * @return VkSwapchainKHR    Vulkan swapchain
     */
    VkSwapchainKHR get() const {
        return m_vk_swapchain;
    }

    /**
     * @brief Get the backbuffer count
     * @return ui32    Number of backbuffers
     */
    ui32 get_backbuffer_count() const {
        return to_ui32(m_backbuffers.size());
    }

    /**
     * @brief Get the backbuffers
     * @return image::s_list const&    List of backbuffer images
     */
    image::s_list const& get_backbuffers() const {
        return m_backbuffers;
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
        return m_v_sync_active;
    }

    /**
     * @brief Check if VK_PRESENT_MODE_MAILBOX_KHR is preferred over VK_PRESENT_MODE_IMMEDIATE_KHR
     * @return VK_PRESENT_MODE_MAILBOX_KHR preferred over VK_PRESENT_MODE_IMMEDIATE_KHR or not
     */
    bool triple_buffer() const {
        return m_triple_buffer_active;
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
    device::ptr m_device = nullptr;

    /// Vulkan surface
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;

    /// Surface format
    VkSurfaceFormatKHR m_format = {};

    /// Vulkan swapchain
    VkSwapchainKHR m_vk_swapchain = VK_NULL_HANDLE;

    /// List of backbuffer images
    image::s_list m_backbuffers;

    /// Size of swapchain
    uv2 m_size;

    /// Reload request active state
    bool m_reload_request_active = false;

    /// V-Sync active state
    bool m_v_sync_active = false;

    /// Prefer VK_PRESENT_MODE_MAILBOX_KHR over VK_PRESENT_MODE_IMMEDIATE_KHR state
    bool m_triple_buffer_active = true;

    /// List of swapchain callbacks
    callback::list m_callbacks;
};

} // namespace lava
