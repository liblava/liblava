/**
 * @file         liblava/frame/render_target.cpp
 * @brief        Render target
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/frame/frame.hpp>
#include <liblava/frame/render_target.hpp>
#include <liblava/frame/window.hpp>

namespace lava {

//-----------------------------------------------------------------------------
bool render_target::create(device_ptr device, VkSurfaceKHR surface, VkSurfaceFormatKHR format, uv2 size, bool v_sync) {
    if (!target.create(device, surface, format, size, v_sync))
        return false;

    swapchain_callback.on_created = [&]() {
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

    swapchain_callback.on_destroyed = [&]() {
        if (on_swapchain_stop)
            on_swapchain_stop();

        for (auto& callback : target_callbacks)
            callback->on_destroyed();

        if (on_destroy_attachments)
            on_destroy_attachments();
    };

    target.add_callback(&swapchain_callback);

    return true;
}

//-----------------------------------------------------------------------------
void render_target::destroy() {
    target_callbacks.clear();

    target.remove_callback(&swapchain_callback);
    target.destroy();
}

//-----------------------------------------------------------------------------
render_target::ptr create_target(window* window, device_ptr device, bool v_sync, surface_format_request request) {
    auto surface = window->create_surface();
    if (!surface)
        return nullptr;

    if (!device->surface_supported(surface))
        return nullptr;

    auto surface_format = get_surface_format(device->get_vk_physical_device(), surface, request);
    if (surface_format.format == VK_FORMAT_UNDEFINED) {
        vkDestroySurfaceKHR(instance::get(), surface, memory::alloc());
        return nullptr;
    }

    auto width = 0u;
    auto height = 0u;
    window->get_framebuffer_size(width, height);

    auto target = std::make_shared<render_target>();
    if (!target->create(device, surface, surface_format, { width, height }, v_sync))
        return nullptr;

    auto target_ptr = target.get();

    window->on_resize = [&, target_ptr](ui32 new_width, ui32 new_height) { return target_ptr->resize({ new_width, new_height }); };

    return target;
}

} // namespace lava
