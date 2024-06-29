/**
 * @file         liblava/frame/renderer.hpp
 * @brief        Plain renderer
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/frame/swapchain.hpp"
#include <optional>

namespace lava {

/// Optional frame index
using optional_index = std::optional<index>;

/**
 * @brief Plain renderer
 */
struct renderer : entity {
    /// Pointer to renderer
    using ptr = renderer*;

    /**
     * @brief Create a new renderer
     * @param target    Swapchain target
     * @return Create was successful or failed
     */
    bool create(swapchain* target);

    /**
     * @brief Destroy the renderer
     */
    void destroy();

    /**
     * @brief Begin to render a frame
     * @return optional_index    Frame index
     */
    optional_index begin_frame();

    /**
     * @brief End of frame rendering
     * @param cmd_buffers    List of command buffers
     * @return End was successful or failed
     */
    bool end_frame(VkCommandBuffers const& cmd_buffers);

    /**
     * @brief Render a frame
     * @param cmd_buffers    List of command buffers
     * @return Render was successful or failed
     */
    bool frame(VkCommandBuffers const& cmd_buffers) {
        if (!begin_frame())
            return false;

        return end_frame(cmd_buffers);
    }

    /**
     * @brief Get the current frame index
     * @return index    Frame index
     */
    index get_frame() const {
        return m_current_frame;
    }

    /**
     * @brief Get the device
     * @return device::ptr    Vulkan device
     */
    device::ptr get_device() {
        return m_device;
    }

    /// The frame waits additionally for these semaphores (Usefully for additional CommandBuffers)
    VkSemaphores user_frame_wait_semaphores;

    /// To user_frame_wait_semaphores corresponding pipeline wait stages
    VkPipelineStageFlagsList user_frame_wait_stages;

    /// The frame additionally signals these semaphores (Usefully for additional CommandBuffers)
    VkSemaphores user_frame_signal_semaphores;

    /// Destroy function
    using destroy_func = std::function<void()>;

    /// Called on renderer destroy
    destroy_func on_destroy;

    /// Active state
    bool active = true;

private:
    /// Vulkan device
    device::ptr m_device = nullptr;

    /// Graphics queue
    queue m_graphics_queue;

    /// Swapchain target
    swapchain* m_target = nullptr;

    /// Current frame index
    index m_current_frame = 0;

    /// Number of queued frames
    ui32 m_queued_frames = 2;

    /// Current sync frame
    ui32 m_current_sync = 0;

    /// List of fences
    VkFences m_fences = {};

    /// List of fences in use
    VkFences m_fences_in_use = {};

    /// List of image acquired semaphores
    VkSemaphores m_image_acquired_semaphores = {};

    /// List of render complete semaphores
    VkSemaphores m_render_complete_semaphores = {};
};

} // namespace lava
