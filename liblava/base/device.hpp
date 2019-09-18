// file      : liblava/base/device.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/base.hpp>
#include <liblava/base/memory.hpp>

namespace lava {

// fwd
struct physical_device;

struct device : no_copy_no_move {

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

    ~device();

    bool create(create_param const& param);
    void destroy();

    struct queue {

        using list = std::vector<queue>;
        using ref = queue const&;

        VkQueue vk_queue = nullptr;
        ui32 family_index = 0;
    };

    queue::ref get_graphics_queue(ui32 index = 0) const { return get_graphics_queues().at(index); }
    queue::ref get_compute_queue(ui32 index = 0) const { return get_compute_queues().at(index); }
    queue::ref get_transfer_queue(ui32 index = 0) const { return get_transfer_queues().at(index); }

    queue::list const& get_graphics_queues() const { return graphics_queues; }
    queue::list const& get_compute_queues() const { return compute_queues; }
    queue::list const& get_transfer_queues() const { return transfer_queues; }

    VkDevice get() const { return vk_device; }
    VolkDeviceTable const& call() const { return table; }

    bool wait_for_idle() const { return check(call().vkDeviceWaitIdle(vk_device)); }

    VkDescriptorPool get_descriptor_pool() const { return descriptor_pool; }

    physical_device const* get_physical_device() const { return _physical_device; }

    VkPhysicalDeviceFeatures const& get_features() const;
    VkPhysicalDeviceProperties const& get_properties() const;

    bool is_surface_supported(VkSurfaceKHR surface) const;

    void set_allocator(allocator::ptr value) { _allocator = value; }
    allocator::ptr get_allocator() { return _allocator; }

    VmaAllocator alloc() const { return _allocator != nullptr ? _allocator->get() : nullptr; }

private:
    bool create_descriptor_pool();

    VkDevice vk_device = nullptr;
    physical_device const* _physical_device = nullptr;

    VolkDeviceTable table = {};

    VkDescriptorPool descriptor_pool = nullptr;

    queue::list graphics_queues;
    queue::list compute_queues;
    queue::list transfer_queues;

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

bool load_and_create_shader_module(device* device, data const& data, VkShaderModule& out);

} // lava
