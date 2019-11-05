// file      : liblava/block/descriptor.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/block/descriptor.hpp>

namespace lava {

descriptor::binding::binding() {

    _binding.binding = 0;
    _binding.descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    _binding.descriptorCount = 0;
    _binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    _binding.pImmutableSamplers = nullptr;
}

descriptor::binding::ptr descriptor::binding::make(index index) {

    auto binding = std::make_shared<descriptor::binding>();

    binding->set(index);
    binding->set_count(1);
    return binding;
}

bool descriptor::create(device* device) {

    dev = device;

    VkDescriptorSetLayoutBindings layoutBindings;

    for (auto& binding : bindings)
        layoutBindings.push_back(binding->get());

    VkDescriptorSetLayoutCreateInfo create_info
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = to_ui32(layoutBindings.size()),
        .pBindings = layoutBindings.data(),
    };
    
    return check(dev->call().vkCreateDescriptorSetLayout(dev->get(), &create_info, memory::alloc(), &layout));
}

void descriptor::destroy() {

    if (!layout)
        return;

    dev->call().vkDestroyDescriptorSetLayout(dev->get(), layout, memory::alloc());
    layout = nullptr;

    // keep device for descriptors
}

void descriptor::add_binding(index binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags) {

    auto item = descriptor::binding::make(binding);
    
    item->set_type(descriptor_type);
    item->set_stage_flags(stage_flags);

    add(item);
}

VkDescriptorSet descriptor::allocate_set() {

    VkDescriptorSet descriptor_set = nullptr;

    VkDescriptorSetAllocateInfo alloc_info
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = dev->get_descriptor_pool(),
        .descriptorSetCount = 1,
        .pSetLayouts = &layout,
    };

    if (failed(dev->call().vkAllocateDescriptorSets(dev->get(), &alloc_info, &descriptor_set)))
        return nullptr;

    return descriptor_set;
}

bool descriptor::free_set(VkDescriptorSet descriptor_set) {

    std::array<VkDescriptorSet, 1> const descriptor_sets = { descriptor_set };

    return check(dev->call().vkFreeDescriptorSets(dev->get(), dev->get_descriptor_pool(), 
                                                    to_ui32(descriptor_sets.size()), descriptor_sets.data()));
}

VkDescriptorSets descriptor::allocate_sets(ui32 size) {

    VkDescriptorSets result(size);

    VkDescriptorSetAllocateInfo alloc_info
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = dev->get_descriptor_pool(),
        .descriptorSetCount = size,
        .pSetLayouts = &layout,
    };

    if (failed(dev->call().vkAllocateDescriptorSets(dev->get(), &alloc_info, result.data())))
        return {};

    return result;
}

bool descriptor::free_sets(VkDescriptorSets const& descriptor_sets) {

    return check(dev->call().vkFreeDescriptorSets(dev->get(), dev->get_descriptor_pool(), 
                                                    to_ui32(descriptor_sets.size()), descriptor_sets.data()));
}

} // lava
