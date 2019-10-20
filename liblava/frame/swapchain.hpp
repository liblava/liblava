// file      : liblava/frame/swapchain.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/resource/image.hpp>

namespace lava {

struct swapchain {

    bool create(device* device, VkSurfaceKHR surface, uv2 size, bool v_sync = false);

    void destroy();

    bool resize(uv2 new_size);

    void request_reload() { reload_request = true; }
    bool must_reload() const { return reload_request; }

    device* get_device() { return dev; }

    uv2 get_size() const { return size; }
    VkFormat get_format() const { return format.format; }

    VkSwapchainKHR get() const { return vk_swapchain; }

    ui32 get_backbuffer_count() const { return to_ui32(backbuffers.size()); }
    image::list const& get_backbuffers() const { return backbuffers; }

    struct callback {

        using list = std::vector<callback*>;

        using created_func = std::function<bool()>;
        created_func on_created;

        using destroyed_func = std::function<void()>;
        destroyed_func on_destroyed;
    };

    void add_callback(callback* cb);
    void remove_callback(callback* cb);

    bool has_v_sync() const { return v_sync; }

private:
    void set_surface_format();

    VkPresentModeKHR choose_present_mode(VkPresentModeKHRs const& present_modes) const;

    bool create_internal();

    void destroy_internal();
    void destroy_backbuffer_views();

    device* dev = nullptr;

    VkSurfaceKHR surface = nullptr;
    VkSurfaceFormatKHR format = {};

    VkSwapchainKHR vk_swapchain = nullptr;

    image::list backbuffers;

    uv2 size;
    bool reload_request = false;

    bool v_sync = false;

    callback::list callbacks;
};

} // lava
