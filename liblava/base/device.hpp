// file      : liblava/base/device.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device_table.hpp>
#include <liblava/base/queue.hpp>
#include <liblava/core/data.hpp>

namespace lava {

    // fwd
    struct physical_device;
    using physical_device_cptr = physical_device const*;

    struct device : device_table {
        using ptr = std::shared_ptr<device>;
        using list = std::vector<device::ptr>;

        struct create_param {
            using ref = create_param const&;

            physical_device_cptr physical_device = nullptr;

            names extensions;
            VkPhysicalDeviceFeatures features{};
            void const* next = nullptr; // pNext

            queue_family_info::list queue_family_infos;

            void add_swapchain_extension() {
                extensions.push_back("VK_KHR_swapchain");
            }

            void set_default_queues() {
                lava::set_default_queues(queue_family_infos);
            }

            void set_all_queues();

            bool add_queue(VkQueueFlags flags, r32 priority = 1.f) {
                return add_queues(flags, 1, priority);
            }
            bool add_queues(VkQueueFlags flags, ui32 count, r32 priority = 1.f);

            bool add_dedicated_queues(r32 priority = 1.f);

            verify_queues_result verify_queues() const;
        };

        ~device() {
            destroy();
        }

        bool create(create_param::ref param);
        void destroy();

        queue::ref get_graphics_queue(index index = 0) const {
            return get_graphics_queues().at(index);
        }
        queue::ref graphics_queue(index index = 0) const {
            return get_graphics_queue(index);
        }

        queue::ref get_compute_queue(index index = 0) const {
            return get_compute_queues().at(index);
        }
        queue::ref compute_queue(index index = 0) const {
            return get_compute_queue(index);
        }

        queue::ref get_transfer_queue(index index = 0) const {
            return get_transfer_queues().at(index);
        }
        queue::ref transfer_queue(index index = 0) const {
            return get_transfer_queue(index);
        }

        queue::list const& get_graphics_queues() const {
            return graphics_queue_list;
        }
        queue::list const& graphics_queues() const {
            return get_graphics_queues();
        }

        queue::list const& get_compute_queues() const {
            return compute_queue_list;
        }
        queue::list const& compute_queues() const {
            return get_compute_queues();
        }

        queue::list const& get_transfer_queues() const {
            return transfer_queue_list;
        }
        queue::list const& transfer_queues() const {
            return get_transfer_queues();
        }

        queue::list const& get_queues() const {
            return queue_list;
        }

        queue::list const& queues() const {
            return get_queues();
        }

        VkDevice get() const {
            return vk_device;
        }
        VolkDeviceTable const& call() const {
            return table;
        }

        bool wait_for_idle() const {
            return check(call().vkDeviceWaitIdle(vk_device));
        }

        physical_device_cptr get_physical_device() const {
            return physical_device;
        }
        VkPhysicalDevice get_vk_physical_device() const;

        VkPhysicalDeviceFeatures const& get_features() const;
        VkPhysicalDeviceProperties const& get_properties() const;

        bool surface_supported(VkSurfaceKHR surface) const;

        void set_allocator(allocator::ptr value) {
            mem_allocator = value;
        }
        allocator::ptr get_allocator() {
            return mem_allocator;
        }

        VmaAllocator alloc() const {
            return mem_allocator != nullptr ? mem_allocator->get() : nullptr;
        }

    private:
        physical_device_cptr physical_device = nullptr;

        queue::list graphics_queue_list;
        queue::list compute_queue_list;
        queue::list transfer_queue_list;
        queue::list queue_list;

        VkPhysicalDeviceFeatures features{};

        allocator::ptr mem_allocator;
    };

    struct device_manager {
        device::ptr create(index physical_device = 0);

        device::ptr create(device::create_param::ref param);

        device::list const& get_all() const {
            return list;
        }

        void wait_idle();

        void clear();

        using create_param_func = std::function<void(device::create_param&)>;
        create_param_func on_create_param;

    private:
        device::list list;
    };

    using device_ptr = device*;

    VkShaderModule create_shader_module(device_ptr device, data const& data);

    using one_time_command_func = std::function<void(VkCommandBuffer)>;
    bool one_time_command_buffer(device_ptr device, VkCommandPool pool, queue::ref queue, one_time_command_func callback);

} // namespace lava
