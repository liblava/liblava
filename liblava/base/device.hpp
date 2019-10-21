// file      : liblava/base/device.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device_table.hpp>

namespace lava {

// fwd
struct physical_device;

struct device : device_table, no_copy_no_move {

    using ptr = std::shared_ptr<device>;
    using list = std::vector<device::ptr>;

    struct create_param {

        const physical_device* _physical_device = nullptr;

        struct queue_info {

            using list = std::vector<queue_info>;

            VkQueueFlags flags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;

            using priority_list = std::vector<float>;
            priority_list priorities;

            ui32 count() const { return to_ui32(priorities.size()); }

            explicit queue_info(ui32 count = 1) {

                for (auto i = 0u; i < count; ++i)
                    priorities.push_back(1.f);
            }
        };

        queue_info::list queue_info_list;

        names extensions;

        void set_default_queues() {

            extensions.push_back("VK_KHR_swapchain");
            queue_info_list.resize(1);
        }
    };

    ~device() { destroy(); }

    bool create(create_param const& param);
    void destroy();

    struct queue {

        using list = std::vector<queue>;
        using ref = queue const&;

        VkQueue vk_queue = nullptr;
        index family = 0;
    };

    queue::ref get_graphics_queue(index index = 0) const { return get_graphics_queues().at(index); }
    queue::ref graphics_queue(index index = 0) const { return get_graphics_queue(index); }

    queue::ref get_compute_queue(index index = 0) const { return get_compute_queues().at(index); }
    queue::ref compute_queue(index index = 0) const { return get_compute_queue(index); }

    queue::ref get_transfer_queue(index index = 0) const { return get_transfer_queues().at(index); }
    queue::ref transfer_queue(index index = 0) const { return get_transfer_queue(index); }

    queue::list const& get_graphics_queues() const { return _graphics_queues; }
    queue::list const& graphics_queues() const { return get_graphics_queues(); }

    queue::list const& get_compute_queues() const { return _compute_queues; }
    queue::list const& compute_queues() const { return get_compute_queues(); }

    queue::list const& get_transfer_queues() const { return _transfer_queues; }
    queue::list const& transfer_queues() const { return get_transfer_queues(); }

    VkDevice get() const { return vk_device; }
    VolkDeviceTable const& call() const { return table; }

    bool wait_for_idle() const { return check(call().vkDeviceWaitIdle(vk_device)); }

    VkDescriptorPool get_descriptor_pool() const { return descriptor_pool; }

    physical_device const* get_physical_device() const { return _physical_device; }
    VkPhysicalDevice get_vk_physical_device() const;

    VkPhysicalDeviceFeatures const& get_features() const;
    VkPhysicalDeviceProperties const& get_properties() const;

    bool is_surface_supported(VkSurfaceKHR surface) const;

    void set_allocator(allocator::ptr value) { _allocator = value; }
    allocator::ptr get_allocator() { return _allocator; }

    VmaAllocator alloc() const { return _allocator != nullptr ? _allocator->get() : nullptr; }

private:
    bool create_descriptor_pool();

    physical_device const* _physical_device = nullptr;

    VkDescriptorPool descriptor_pool = nullptr;

    queue::list _graphics_queues;
    queue::list _compute_queues;
    queue::list _transfer_queues;

    allocator::ptr _allocator;
};

struct device_manager {

    device::ptr create();

    device::ptr create(device::create_param const& param);

    device::list const& get_all() const { return list; }

    void wait_idle();

    void clear();

private:
    device::list list;
};

VkShaderModule create_shader_module(device* device, data const& data);

} // lava
