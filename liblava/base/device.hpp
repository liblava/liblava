// file      : liblava/base/device.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device_table.hpp>

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

    bool create(create_param::ref param);
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

    queue::list const& get_graphics_queues() const { return graphics_queue_list; }
    queue::list const& graphics_queues() const { return get_graphics_queues(); }

    queue::list const& get_compute_queues() const { return compute_queue_list; }
    queue::list const& compute_queues() const { return get_compute_queues(); }

    queue::list const& get_transfer_queues() const { return transfer_queue_list; }
    queue::list const& transfer_queues() const { return get_transfer_queues(); }

    VkDevice get() const { return vk_device; }
    VolkDeviceTable const& call() const { return table; }

    bool wait_for_idle() const { return check(call().vkDeviceWaitIdle(vk_device)); }

    VkDescriptorPool get_descriptor_pool() const { return descriptor_pool; }

    physical_device_cptr get_physical_device() const { return physical_device; }
    VkPhysicalDevice get_vk_physical_device() const;

    VkPhysicalDeviceFeatures const& get_features() const;
    VkPhysicalDeviceProperties const& get_properties() const;

    bool surface_supported(VkSurfaceKHR surface) const;

    void set_allocator(allocator::ptr value) { mem_allocator = value; }
    allocator::ptr get_allocator() { return mem_allocator; }

    VmaAllocator alloc() const { return mem_allocator != nullptr ? mem_allocator->get() : nullptr; }

private:
    bool create_descriptor_pool();

    physical_device_cptr physical_device = nullptr;

    VkDescriptorPool descriptor_pool = 0;

    device::queue::list graphics_queue_list;
    device::queue::list compute_queue_list;
    device::queue::list transfer_queue_list;

    allocator::ptr mem_allocator;
};

struct device_manager {

    device::ptr create(index physical_device = 0);

    device::ptr create(device::create_param::ref param);

    device::list const& get_all() const { return list; }

    void wait_idle();

    void clear();

private:
    device::list list;
};

using device_ptr = device*;

VkShaderModule create_shader_module(device_ptr device, data const& data);

} // lava
