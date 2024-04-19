/**
 * @file         liblava/block/pipeline_layout.cpp
 * @brief        Pipeline layout
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/block/pipeline_layout.hpp"
#include <array>

namespace lava {

//-----------------------------------------------------------------------------
bool pipeline_layout::create(device::ptr dev) {
    device = dev;

    VkDescriptorSetLayouts layouts;
    for (auto& layout : descriptors)
        layouts.push_back(layout->get());

    VkPipelineLayoutCreateInfo const pipelineLayoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = to_ui32(layouts.size()),
        .pSetLayouts = layouts.data(),
        .pushConstantRangeCount = to_ui32(push_constant_ranges.size()),
        .pPushConstantRanges = push_constant_ranges.data(),
    };

    return check(device->call().vkCreatePipelineLayout(device->get(),
                                                       &pipelineLayoutInfo,
                                                       memory::instance().alloc(),
                                                       &layout));
}

//-----------------------------------------------------------------------------
void pipeline_layout::destroy() {
    if (!layout)
        return;

    device->call().vkDestroyPipelineLayout(device->get(),
                                           layout,
                                           memory::instance().alloc());
    layout = VK_NULL_HANDLE;

    clear();
}

//-----------------------------------------------------------------------------
void pipeline_layout::bind_descriptor_set(VkCommandBuffer cmd_buf,
                                          VkDescriptorSet descriptor_set,
                                          index first_set,
                                          offset_list offsets,
                                          VkPipelineBindPoint bind_point) {
    std::array<VkDescriptorSet, 1> const descriptor_sets = {descriptor_set};

    device->call().vkCmdBindDescriptorSets(cmd_buf,
                                           bind_point,
                                           layout,
                                           first_set,
                                           to_ui32(descriptor_sets.size()),
                                           descriptor_sets.data(),
                                           to_ui32(offsets.size()),
                                           offsets.data());
}

} // namespace lava
