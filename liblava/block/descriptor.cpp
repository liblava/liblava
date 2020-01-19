// file      : liblava/block/descriptor.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/block/descriptor.hpp>

namespace lava {

descriptor::binding::binding() {

    vk_binding.binding = 0;
    vk_binding.descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    vk_binding.descriptorCount = 0;
    vk_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    vk_binding.pImmutableSamplers = nullptr;
}

bool descriptor::create(device_ptr device_) {

    device = device_;

    VkDescriptorSetLayoutBindings layoutBindings;

    for (auto& binding : bindings)
        layoutBindings.push_back(binding->get());

    VkDescriptorSetLayoutCreateInfo create_info
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = to_ui32(layoutBindings.size()),
        .pBindings = layoutBindings.data(),
    };
    
    return check(device->call().vkCreateDescriptorSetLayout(device->get(), &create_info, memory::alloc(), &layout));
}

void descriptor::destroy() {

    if (!layout)
        return;

    device->call().vkDestroyDescriptorSetLayout(device->get(), layout, memory::alloc());
    layout = 0;

    // keep device for descriptors
}

void descriptor::add_binding(index binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags) {

    auto item = make_descriptor_binding(binding);
    
    item->set_type(descriptor_type);
    item->set_stage_flags(stage_flags);

    add(item);
}

VkDescriptorSet descriptor::allocate_set() {

    VkDescriptorSet descriptor_set = 0;

    VkDescriptorSetAllocateInfo const alloc_info
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = device->get_descriptor_pool(),
        .descriptorSetCount = 1,
        .pSetLayouts = &layout,
    };

    if (failed(device->call().vkAllocateDescriptorSets(device->get(), &alloc_info, &descriptor_set)))
        return 0;

    return descriptor_set;
}

bool descriptor::free_set(VkDescriptorSet descriptor_set) {

    std::array<VkDescriptorSet, 1> const descriptor_sets = { descriptor_set };

    return check(device->call().vkFreeDescriptorSets(device->get(), device->get_descriptor_pool(), 
                                                    to_ui32(descriptor_sets.size()), descriptor_sets.data()));
}

VkDescriptorSets descriptor::allocate_sets(ui32 size) {

    VkDescriptorSets result(size);

    VkDescriptorSetAllocateInfo const alloc_info
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = device->get_descriptor_pool(),
        .descriptorSetCount = size,
        .pSetLayouts = &layout,
    };

    if (failed(device->call().vkAllocateDescriptorSets(device->get(), &alloc_info, result.data())))
        return {};

    return result;
}

bool descriptor::free_sets(VkDescriptorSets const& descriptor_sets) {

    return check(device->call().vkFreeDescriptorSets(device->get(), device->get_descriptor_pool(), 
                                                    to_ui32(descriptor_sets.size()), descriptor_sets.data()));
}

descriptor::binding::ptr make_descriptor_binding(index index) {

    auto binding = std::make_shared<descriptor::binding>();

    binding->set(index);
    binding->set_count(1);
    return binding;
}

} // lava
