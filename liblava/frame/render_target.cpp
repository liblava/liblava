// file      : liblava/frame/render_target.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/frame/render_target.hpp>
#include <liblava/frame/window.hpp>
#include <liblava/frame/frame.hpp>

namespace lava {

bool render_target::create(device* device, VkSurfaceKHR surface, uv2 size) {

    if (!_swapchain.create(device, surface, size))
        return false;

    _swapchain_callback.on_created = [&]() {

        if (on_create_attachments) {

            auto target_attachments = on_create_attachments();

            for (auto& callback : target_callbacks)
                if (!callback->on_created(target_attachments, { {}, get_size() }))
                    return false;
        }

        if (on_swapchain_start)
            if (!on_swapchain_start())
                return false;

        return true;
    };

    _swapchain_callback.on_destroyed = [&]() {

        if (on_swapchain_stop)
            on_swapchain_stop();

        for (auto& callback : target_callbacks)
            callback->on_destroyed();

        if (on_destroy_attachments)
            on_destroy_attachments();
    };

    _swapchain.add_callback(&_swapchain_callback);

    return true;
}

void render_target::destroy() {

    target_callbacks.clear();

    _swapchain.remove_callback(&_swapchain_callback);
    _swapchain.destroy();
}

} // lava

lava::render_target::ptr lava::create_target(window* window, device* device) {

    auto surface = window->create_surface();
    if (!surface)
        return nullptr;

    if (!device->is_surface_supported(surface))
        return nullptr;

    auto width = 0u;
    auto height = 0u;
    window->get_framebuffer_size(width, height);

    auto target = std::make_shared<render_target>();
    if (!target->create(device, surface, { width, height }))
        return nullptr;

    auto target_ptr = target.get();

    window->on_resize = [&, target_ptr](ui32 new_width, ui32 new_height) { return target_ptr->resize({ new_width, new_height }); };

    return target;
}
