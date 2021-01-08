// file      : liblava/frame/swapchain.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/resource/image.hpp>

namespace lava {

    struct swapchain : id_obj {
        bool create(device_ptr device, VkSurfaceKHR surface, uv2 size, bool v_sync = false);

        void destroy();

        bool resize(uv2 new_size);

        void request_reload() {
            reload_request_active = true;
        }
        bool reload_request() const {
            return reload_request_active;
        }

        device_ptr get_device() {
            return device;
        }

        uv2 get_size() const {
            return size;
        }
        VkFormat get_format() const {
            return format.format;
        }

        VkSwapchainKHR get() const {
            return vk_swapchain;
        }

        ui32 get_backbuffer_count() const {
            return to_ui32(backbuffers.size());
        }
        image::list const& get_backbuffers() const {
            return backbuffers;
        }

        struct callback {
            using list = std::vector<callback*>;

            using created_func = std::function<bool()>;
            created_func on_created;

            using destroyed_func = std::function<void()>;
            destroyed_func on_destroyed;
        };

        void add_callback(callback* cb);
        void remove_callback(callback* cb);

        bool v_sync() const {
            return v_sync_active;
        }

        bool surface_supported(index queue_family) const;

    private:
        void set_surface_format();

        VkPresentModeKHR choose_present_mode(VkPresentModeKHRs const& present_modes) const;
        VkSwapchainCreateInfoKHR create_info(VkPresentModeKHRs present_modes);

        bool create_internal();
        void destroy_internal();
        void destroy_backbuffer_views();

        device_ptr device = nullptr;

        VkSurfaceKHR surface = 0;
        VkSurfaceFormatKHR format = {};
        VkSwapchainKHR vk_swapchain = 0;

        image::list backbuffers;

        uv2 size;
        bool reload_request_active = false;
        bool v_sync_active = false;

        callback::list callbacks;
    };

} // namespace lava
