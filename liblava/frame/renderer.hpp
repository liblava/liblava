// file      : liblava/frame/renderer.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/frame/swapchain.hpp>

#include <optional>

namespace lava {

struct renderer {

    bool create(swapchain* target);
    void destroy();

    std::optional<index> begin_frame();
    bool end_frame(VkCommandBuffers const& cmd_buffers);

    bool frame(VkCommandBuffers const& cmd_buffers) {

        if (!begin_frame())
            return false;

        return end_frame(cmd_buffers);
    }

    index get_current_frame() const { return frame_index; }

    using destroy_func = std::function<void()>;
    destroy_func on_destroy;

    bool active = true;

private:
    device* dev = nullptr;
    device::queue queue;

    swapchain* target = nullptr;

    index frame_index = 0;
    ui32 queued_frames = 2;

    ui32 current_sync = 0;
    VkFences fences = {};
    VkSemaphores image_acquired_semaphores = {};
    VkSemaphores render_complete_semaphores = {};
};

} // lava
