/**
 * @file         liblava/block/descriptor.cpp
 * @brief        Descriptor definition
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/block/descriptor.hpp"
#include <array>

namespace lava {

//-----------------------------------------------------------------------------
descriptor::binding::binding() {
    vk_binding.binding = 0;
    vk_binding.descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    vk_binding.descriptorCount = 0;
    vk_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    vk_binding.pImmutableSamplers = nullptr;
}

//-----------------------------------------------------------------------------
bool descriptor::pool::create(device_p d,
                              VkDescriptorPoolSizesRef s,
                              ui32 m,
                              VkDescriptorPoolCreateFlags flags) {
    if (s.empty() || (m == 0))
        return false;

    device = d;
    sizes = s;
    max = m;

    VkDescriptorPoolCreateInfo const pool_info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = flags,
        .maxSets = max,
        .poolSizeCount = to_ui32(s.size()),
        .pPoolSizes = sizes.data(),
    };

    return check(device->call().vkCreateDescriptorPool(device->get(),
                                                       &pool_info,
                                                       memory::instance().alloc(),
                                                       &vk_pool));
}

//-----------------------------------------------------------------------------
void descriptor::pool::destroy() {
    device->call().vkDestroyDescriptorPool(device->get(),
                                           vk_pool,
                                           memory::instance().alloc());
    vk_pool = VK_NULL_HANDLE;

    device = nullptr;
    sizes.clear();
    max = 0;
}

//-----------------------------------------------------------------------------
bool descriptor::create(device_p d) {
    device = d;

    VkDescriptorSetLayoutBindings layoutBindings;

    for (auto& binding : bindings)
        layoutBindings.push_back(binding->get());

    VkDescriptorSetLayoutCreateInfo create_info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = to_ui32(layoutBindings.size()),
        .pBindings = layoutBindings.data(),
    };

    return check(device->call().vkCreateDescriptorSetLayout(device->get(),
                                                            &create_info,
                                                            memory::instance().alloc(),
                                                            &layout));
}

//-----------------------------------------------------------------------------
void descriptor::destroy() {
    if (!layout)
        return;

    device->call().vkDestroyDescriptorSetLayout(device->get(),
                                                layout,
                                                memory::instance().alloc());
    layout = VK_NULL_HANDLE;

    clear_bindings();

    // keep device for descriptors
}

//-----------------------------------------------------------------------------
void descriptor::add_binding(index binding,
                             VkDescriptorType descriptor_type,
                             VkShaderStageFlags stage_flags) {
    auto item = binding::make(binding);

    item->set_type(descriptor_type);
    item->set_stage_flags(stage_flags);

    add(item);
}

//-----------------------------------------------------------------------------
VkDescriptorSet descriptor::allocate_set(VkDescriptorPool pool) {
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

    VkDescriptorSetAllocateInfo const alloc_info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &layout,
    };

    if (failed(device->call().vkAllocateDescriptorSets(device->get(),
                                                       &alloc_info,
                                                       &descriptor_set)))
        return 0;

    return descriptor_set;
}

//-----------------------------------------------------------------------------
bool descriptor::free_set(VkDescriptorSet& descriptor_set,
                          VkDescriptorPool pool) {
    std::array<VkDescriptorSet, 1> const descriptor_sets = {descriptor_set};

    auto result = check(device->call().vkFreeDescriptorSets(device->get(),
                                                            pool,
                                                            to_ui32(descriptor_sets.size()),
                                                            descriptor_sets.data()));
    if (result)
        descriptor_set = VK_NULL_HANDLE;
    return result;
}

//-----------------------------------------------------------------------------
VkDescriptorSets descriptor::allocate_sets(ui32 size,
                                           VkDescriptorPool pool) {
    VkDescriptorSets result(size);

    VkDescriptorSetAllocateInfo const alloc_info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = pool,
        .descriptorSetCount = size,
        .pSetLayouts = &layout,
    };

    if (failed(device->call().vkAllocateDescriptorSets(device->get(),
                                                       &alloc_info,
                                                       result.data())))
        return {};

    return result;
}

//-----------------------------------------------------------------------------
bool descriptor::free_sets(VkDescriptorSets& descriptor_sets,
                           VkDescriptorPool pool) {
    auto result = check(device->call().vkFreeDescriptorSets(device->get(),
                                                            pool,
                                                            to_ui32(descriptor_sets.size()),
                                                            descriptor_sets.data()));

    if (result)
        descriptor_sets.clear();
    return result;
}

} // namespace lava
