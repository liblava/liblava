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
        return current_frame;
    }

    /**
     * @brief Get the device
     * @return device_p    Vulkan device
     */
    device_p get_device() {
        return device;
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
    device_p device = nullptr;

    /// Graphics queue
    queue graphics_queue;

    /// Swapchain target
    swapchain* target = nullptr;

    /// Current frame index
    index current_frame = 0;

    /// Number of queued frames
    ui32 queued_frames = 2;

    /// Current sync frame
    ui32 current_sync = 0;

    /// List of fences
    VkFences fences = {};

    /// List of fences in use
    VkFences fences_in_use = {};

    /// List of image acquired semaphores
    VkSemaphores image_acquired_semaphores = {};

    /// List of render complete semaphores
    VkSemaphores render_complete_semaphores = {};
};

/// Renderer type
using renderer_t = renderer;

} // namespace lava
