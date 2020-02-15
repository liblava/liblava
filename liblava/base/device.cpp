// file      : liblava/base/device.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/base/device.hpp>
#include <liblava/base/instance.hpp>
#include <liblava/base/physical_device.hpp>

namespace lava {

bool device::create(create_param::ref param) {

    physical_device = param.physical_device;
    if (!physical_device)
        return false;

    std::vector<VkDeviceQueueCreateInfo> queue_create_info_list(param.queue_info_list.size());

    for (size_t i = 0, e = param.queue_info_list.size(); i != e; ++i) {

        queue_create_info_list[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

        ui32 index = 0;
        if (!physical_device->get_queue_family(index, param.queue_info_list[i].flags)) {

            log()->error("create device queue family");
            return false;
        }

        queue_create_info_list[i].queueFamilyIndex = index;
        queue_create_info_list[i].queueCount = param.queue_info_list[i].count();
        queue_create_info_list[i].pQueuePriorities = param.queue_info_list[i].priorities.data();
    }

    VkDeviceCreateInfo create_info
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = to_ui32(queue_create_info_list.size()),
        .pQueueCreateInfos = queue_create_info_list.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = to_ui32(param.extensions.size()),
        .ppEnabledExtensionNames = param.extensions.data(),
        .pEnabledFeatures = &physical_device->get_features(),
    };

    if (failed(vkCreateDevice(physical_device->get(), &create_info, memory::alloc(), &vk_device))) {

        log()->error("create device");
        return false;
    }

    load_table();

    graphics_queue_list.clear();
    compute_queue_list.clear();
    transfer_queue_list.clear();

    index_map queue_family_map;

    for (size_t i = 0, ei = queue_create_info_list.size(); i != ei; ++i) {

        if (!queue_family_map.count(queue_create_info_list[i].queueFamilyIndex))
            queue_family_map.emplace(queue_create_info_list[i].queueFamilyIndex, 0);

        for (size_t j = 0, ej = queue_create_info_list[i].queueCount; j != ej; ++j) {

            auto counter = queue_family_map[queue_create_info_list[i].queueFamilyIndex];
            queue_family_map[queue_create_info_list[i].queueFamilyIndex]++;

            VkQueue queue = nullptr;
            call().vkGetDeviceQueue(vk_device, queue_create_info_list[i].queueFamilyIndex, counter, &queue);

            if (param.queue_info_list[i].flags & VK_QUEUE_GRAPHICS_BIT)
                graphics_queue_list.push_back({ queue, queue_create_info_list[i].queueFamilyIndex });
            if (param.queue_info_list[i].flags & VK_QUEUE_COMPUTE_BIT)
                compute_queue_list.push_back({ queue, queue_create_info_list[i].queueFamilyIndex });
            if (param.queue_info_list[i].flags & VK_QUEUE_TRANSFER_BIT)
                transfer_queue_list.push_back({ queue, queue_create_info_list[i].queueFamilyIndex });
        }
    }

    return create_descriptor_pool();
}

void device::destroy() {

    if (!vk_device)
        return;

    graphics_queue_list.clear();
    compute_queue_list.clear();
    transfer_queue_list.clear();

    call().vkDestroyDescriptorPool(vk_device, descriptor_pool, memory::alloc());
    descriptor_pool = 0;

    mem_allocator = nullptr;

    call().vkDestroyDevice(vk_device, memory::alloc());
    vk_device = nullptr;

    table = {};
}

bool device::create_descriptor_pool() {

    auto const count = 11u;
    auto const size = 1000u;

    VkDescriptorPoolSize const pool_size[count] = 
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, size },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, size },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, size },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, size },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, size },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, size },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, size },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, size },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, size },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, size },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, size },
    };

    VkDescriptorPoolCreateInfo const pool_info
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = size * count,
        .poolSizeCount = count,
        .pPoolSizes = pool_size,
    };

    return check(call().vkCreateDescriptorPool(vk_device, &pool_info, memory::alloc(), &descriptor_pool));
}

bool device::surface_supported(VkSurfaceKHR surface) const {

    return physical_device->surface_supported(get_graphics_queue().family, surface);
}

VkPhysicalDevice device::get_vk_physical_device() const {

    return physical_device->get();
}

VkPhysicalDeviceFeatures const& device::get_features() const { return physical_device->get_features(); }

VkPhysicalDeviceProperties const& device::get_properties() const { return physical_device->get_properties(); }

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

    auto allocator = make_allocator(result->get_vk_physical_device(), result->get());
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

} // lava

VkShaderModule lava::create_shader_module(device_ptr device, data const& data) {

    VkShaderModuleCreateInfo shader_module_create_info
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = data.size,
        .pCode = reinterpret_cast<ui32*>(data.ptr),
    };

    VkShaderModule result;
    if (!device->vkCreateShaderModule(&shader_module_create_info, memory::alloc(), &result))
        return 0;

    return result;
}
