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
    m_vk_binding.binding = 0;
    m_vk_binding.descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    m_vk_binding.descriptorCount = 0;
    m_vk_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    m_vk_binding.pImmutableSamplers = nullptr;
}

//-----------------------------------------------------------------------------
bool descriptor::pool::create(device::ptr dev,
                              VkDescriptorPoolSizesRef s,
                              ui32 m,
                              VkDescriptorPoolCreateFlags flags) {
    if (s.empty() || (m == 0))
        return false;

    m_device = dev;
    m_sizes = s;
    m_max = m;

    VkDescriptorPoolCreateInfo const pool_info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = flags,
        .maxSets = m_max,
        .poolSizeCount = to_ui32(s.size()),
        .pPoolSizes = m_sizes.data(),
    };

    return check(m_device->call().vkCreateDescriptorPool(m_device->get(),
                                                         &pool_info,
                                                         memory::instance().alloc(),
                                                         &m_vk_pool));
}

//-----------------------------------------------------------------------------
void descriptor::pool::destroy() {
    m_device->call().vkDestroyDescriptorPool(m_device->get(),
                                             m_vk_pool,
                                             memory::instance().alloc());
    m_vk_pool = VK_NULL_HANDLE;

    m_device = nullptr;
    m_sizes.clear();
    m_max = 0;
}

//-----------------------------------------------------------------------------
bool descriptor::create(device::ptr dev) {
    m_device = dev;

    VkDescriptorSetLayoutBindings layoutBindings;

    for (auto& binding : m_bindings)
        layoutBindings.push_back(binding->get());

    VkDescriptorSetLayoutCreateInfo create_info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = to_ui32(layoutBindings.size()),
        .pBindings = layoutBindings.data(),
    };

    return check(m_device->call().vkCreateDescriptorSetLayout(m_device->get(),
                                                              &create_info,
                                                              memory::instance().alloc(),
                                                              &m_layout));
}

//-----------------------------------------------------------------------------
void descriptor::destroy() {
    if (!m_layout)
        return;

    m_device->call().vkDestroyDescriptorSetLayout(m_device->get(),
                                                  m_layout,
                                                  memory::instance().alloc());
    m_layout = VK_NULL_HANDLE;

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
        .pSetLayouts = &m_layout,
    };

    if (failed(m_device->call().vkAllocateDescriptorSets(m_device->get(),
                                                         &alloc_info,
                                                         &descriptor_set)))
        return 0;

    return descriptor_set;
}

//-----------------------------------------------------------------------------
bool descriptor::deallocate_set(VkDescriptorSet& descriptor_set,
                                VkDescriptorPool pool) {
    std::array<VkDescriptorSet, 1> const descriptor_sets = {descriptor_set};

    auto result = check(m_device->call().vkFreeDescriptorSets(m_device->get(),
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
        .pSetLayouts = &m_layout,
    };

    if (failed(m_device->call().vkAllocateDescriptorSets(m_device->get(),
                                                         &alloc_info,
                                                         result.data())))
        return {};

    return result;
}

//-----------------------------------------------------------------------------
bool descriptor::deallocate_sets(VkDescriptorSets& descriptor_sets,
                                 VkDescriptorPool pool) {
    auto result = check(m_device->call().vkFreeDescriptorSets(m_device->get(),
                                                              pool,
                                                              to_ui32(descriptor_sets.size()),
                                                              descriptor_sets.data()));

    if (result)
        descriptor_sets.clear();
    return result;
}

} // namespace lava
