// file      : liblava/base/device.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#include <liblava/base/device.hpp>
#include <liblava/base/instance.hpp>
#include <liblava/base/physical_device.hpp>

namespace lava {

    void device::create_param::set_all_queues() {
        lava::set_all_queues(queue_family_infos, physical_device->get_queue_family_properties());
    }

    bool device::create_param::add_queues(VkQueueFlags flags, ui32 count, r32 priority) {
        return lava::add_queues(queue_family_infos, physical_device->get_queue_family_properties(), flags, count, priority);
    }

    bool device::create_param::add_dedicated_queues(r32 priority) {
        return lava::add_dedicated_queues(queue_family_infos, physical_device->get_queue_family_properties(), priority);
    }

    verify_queues_result device::create_param::verify_queues() const {
        return lava::verify_queues(queue_family_infos, physical_device->get_queue_family_properties());
    }

    void log_verify_queues_failed(verify_queues_result result) {
        switch (result) {
        case verify_queues_result::empty_list: {
            log()->error("create device - verify queues - param with empty list");
            break;
        }
        case verify_queues_result::no_properties: {
            log()->error("create device - verify queues - no device family properties");
            break;
        }
        case verify_queues_result::duplicate_family_index: {
            log()->error("create device - verify queues - duplicate family index");
            break;
        }
        case verify_queues_result::no_family_index: {
            log()->error("create device - verify queues - family index is not available");
            break;
        }
        case verify_queues_result::no_queues: {
            log()->error("create device - verify queues - undefined queues in family");
            break;
        }
        case verify_queues_result::too_many_queues: {
            log()->error("create device - verify queues - number of queues is incorrect");
            break;
        }
        case verify_queues_result::no_compatible_flags: {
            log()->error("create device - verify queues - no compatible flags in queue");
            break;
        }
        default:
            log()->error("create device - verify queues");
        }
    }

    bool device::create(create_param::ref param) {
        physical_device = param.physical_device;
        if (!physical_device)
            return false;

        auto verify_result = param.verify_queues();
        if (verify_result != verify_queues_result::ok) {
            log_verify_queues_failed(verify_result);
            return false;
        }

        std::vector<VkDeviceQueueCreateInfo> queue_create_info_list(param.queue_family_infos.size());

        std::vector<std::vector<r32>> priorities;
        priorities.resize(param.queue_family_infos.size());

        for (auto i = 0u; i < param.queue_family_infos.size(); ++i) {
            queue_create_info_list[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info_list[i].queueFamilyIndex = param.queue_family_infos[i].family_index;
            queue_create_info_list[i].queueCount = param.queue_family_infos[i].queues.size();

            for (auto j = 0u; j < param.queue_family_infos[i].queues.size(); ++j)
                priorities.at(i).push_back(param.queue_family_infos[i].queues.at(j).priority);

            queue_create_info_list[i].pQueuePriorities = priorities.at(i).data();
        }

        VkDeviceCreateInfo create_info{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = param.next,
            .queueCreateInfoCount = to_ui32(queue_create_info_list.size()),
            .pQueueCreateInfos = queue_create_info_list.data(),
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = to_ui32(param.extensions.size()),
            .ppEnabledExtensionNames = param.extensions.data(),
            .pEnabledFeatures = &param.features,
        };

        if (failed(vkCreateDevice(physical_device->get(), &create_info, memory::alloc(), &vk_device))) {
            log()->error("create device");
            return false;
        }

        features = param.features;

        load_table();

        graphics_queue_list.clear();
        compute_queue_list.clear();
        transfer_queue_list.clear();
        queue_list.clear();

        for (auto i = 0u; i != queue_create_info_list.size(); ++i) {
            auto queue_family_index = queue_create_info_list[i].queueFamilyIndex;
            auto queue_family_flags = physical_device->get_queue_family_properties().at(queue_family_index).queueFlags;

            for (auto queue_index = 0u; queue_index != queue_create_info_list[i].queueCount; ++queue_index) {
                VkQueue vk_queue = nullptr;
                call().vkGetDeviceQueue(vk_device, queue_family_index, queue_index, &vk_queue);

                auto flags = param.queue_family_infos[i].queues.at(queue_index).flags;
                auto priority = queue_create_info_list[i].pQueuePriorities[queue_index];

                queue queue{
                    vk_queue, queue_family_flags, queue_family_index, priority
                };

                if (flags & VK_QUEUE_GRAPHICS_BIT)
                    graphics_queue_list.push_front(queue);
                if (flags & VK_QUEUE_COMPUTE_BIT)
                    compute_queue_list.push_front(queue);
                if (flags & VK_QUEUE_TRANSFER_BIT)
                    transfer_queue_list.push_front(queue);

                queue_list.push_back(queue);
            }
        }

        return true;
    }

    void device::destroy() {
        if (!vk_device)
            return;

        graphics_queue_list.clear();
        compute_queue_list.clear();
        transfer_queue_list.clear();
        queue_list.clear();

        if (mem_allocator) {
            mem_allocator->destroy();
            mem_allocator = nullptr;
        }

        call().vkDestroyDevice(vk_device, memory::alloc());
        vk_device = nullptr;

        table = {};
    }

    bool device::surface_supported(VkSurfaceKHR surface) const {
        for (auto& queue : queue_list) {
            if (physical_device->surface_supported(queue.family, surface))
                return true;
        }

        return false;
    }

    VkPhysicalDevice device::get_vk_physical_device() const {
        return physical_device->get();
    }

    VkPhysicalDeviceFeatures const& device::get_features() const {
        return features;
    }

    VkPhysicalDeviceProperties const& device::get_properties() const {
        return physical_device->get_properties();
    }

    device::ptr device_manager::create(index pd) {
        auto physical_device = &instance::get_first_physical_device();

        if (pd > 0) {
            if (pd >= instance::singleton().get_physical_devices().size()) {
                log()->error("create device - no physical device {}", pd);
                return nullptr;
            }

            physical_device = &instance::singleton().get_physical_devices().at(pd);
        }

        if (!physical_device->swapchain_supported())
            return nullptr;

        auto param = physical_device->create_default_device_param();
        if (on_create_param)
            on_create_param(param);

        return create(param);
    }

    device::ptr device_manager::create(device::create_param::ref param) {
        auto result = std::make_shared<device>();
        if (!result->create(param))
            return nullptr;

        auto allocator = create_allocator(result.get(), param.vma_flags);
        if (!allocator)
            return nullptr;

        result->set_allocator(allocator);

        list.push_back(result);
        return result;
    }

    void device_manager::wait_idle() {
        for (auto& device : list)
            device->wait_for_idle();
    }

    void device_manager::clear() {
        for (auto& device : list)
            device->destroy();

        list.clear();
    }

} // namespace lava

VkShaderModule lava::create_shader_module(device_ptr device, cdata const& data) {
    VkShaderModuleCreateInfo shader_module_create_info{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = data.size,
        .pCode = reinterpret_cast<const ui32*>(data.ptr),
    };

    VkShaderModule result;
    if (!device->vkCreateShaderModule(&shader_module_create_info, memory::alloc(), &result))
        return 0;

    return result;
}

bool lava::one_time_command_buffer(lava::device_ptr device, VkCommandPool pool, lava::queue::ref queue, one_time_command_func callback) {
    if (!callback)
        return false;

    VkCommandBuffer cmd_buf = nullptr;
    if (!device->vkAllocateCommandBuffers(pool, 1, &cmd_buf, VK_COMMAND_BUFFER_LEVEL_PRIMARY))
        return false;

    VkCommandBufferBeginInfo const begin_info = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                  .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };
    if (!check(device->call().vkBeginCommandBuffer(cmd_buf, &begin_info)))
        return false;

    callback(cmd_buf);

    device->call().vkEndCommandBuffer(cmd_buf);

    VkFence fence = VK_NULL_HANDLE;
    VkFenceCreateInfo const fence_info = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    if (!device->vkCreateFence(&fence_info, &fence))
        return false;

    VkSubmitInfo const submit_info = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                       .commandBufferCount = 1,
                                       .pCommandBuffers = &cmd_buf };
    if (!device->vkQueueSubmit(queue.vk_queue, 1, &submit_info, fence)) {
        device->vkDestroyFence(fence);
        return false;
    }

    device->vkWaitForFences(1, &fence, VK_TRUE, ~0);
    device->vkDestroyFence(fence);

    device->vkFreeCommandBuffers(pool, 1, &cmd_buf);

    return true;
}