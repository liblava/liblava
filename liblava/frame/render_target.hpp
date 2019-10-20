// file      : liblava/frame/render_target.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/frame/swapchain.hpp>
#include <liblava/fwd.hpp>

namespace lava {

struct render_target {

    using ptr = std::shared_ptr<render_target>;

    bool create(device* device, VkSurfaceKHR surface, uv2 size, bool v_sync = false);
    void destroy();

    uv2 get_size() const { return _swapchain.get_size(); }
    bool resize(uv2 new_size) { return _swapchain.resize(new_size); }

    ui32 get_frame_count() const { return _swapchain.get_backbuffer_count(); }

    bool must_reload() const { return _swapchain.must_reload(); }
    void reload() { _swapchain.resize(_swapchain.get_size()); }

    device* get_device() { return _swapchain.get_device(); }
    swapchain* get_swapchain() { return &_swapchain; }

    VkFormat get_format() const { return _swapchain.get_format(); }

    image::list const& get_backbuffers() const { return _swapchain.get_backbuffers(); }
    inline image::ptr get_backbuffer(index index) { 

        auto& backbuffers = get_backbuffers();
        if (index >= backbuffers.size())
            return nullptr;

        return backbuffers.at(index);
    }

    inline VkImage get_backbuffer_image(index index) {

        auto result = get_backbuffer(index);
        return result ? result->get() : nullptr;
    }

    void add_callback(target_callback* callback) { target_callbacks.push_back(callback); }
    void remove_callback(target_callback* callback) { remove(target_callbacks, callback); }

    using swapchain__start_func = std::function<bool()>;
    swapchain__start_func on_swapchain_start;

    using swapchain_stop_func = std::function<void()>;
    swapchain_stop_func on_swapchain_stop;

    using create_attachments_func = std::function<VkAttachments()>;
    create_attachments_func on_create_attachments;

    using destroy_attachments_func = std::function<void()>;
    destroy_attachments_func on_destroy_attachments;

private:
    swapchain _swapchain;
    swapchain::callback _swapchain_callback;

    target_callback::list target_callbacks;
};

render_target::ptr create_target(window* window, device* device, bool v_sync = false);

inline render_target::ptr create_target_v_sync(window* window, device* device) { 
    
    return create_target(window, device, true); 
}

} // lava
