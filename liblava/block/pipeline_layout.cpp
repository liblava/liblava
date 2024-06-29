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
    m_device = dev;

    VkDescriptorSetLayouts layouts;
    for (auto& layout : m_descriptors)
        layouts.push_back(layout->get());

    VkPipelineLayoutCreateInfo const pipelineLayoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = to_ui32(layouts.size()),
        .pSetLayouts = layouts.data(),
        .pushConstantRangeCount = to_ui32(m_push_constant_ranges.size()),
        .pPushConstantRanges = m_push_constant_ranges.data(),
    };

    return check(m_device->call().vkCreatePipelineLayout(m_device->get(),
                                                         &pipelineLayoutInfo,
                                                         memory::instance().alloc(),
                                                         &m_layout));
}

//-----------------------------------------------------------------------------
void pipeline_layout::destroy() {
    if (!m_layout)
        return;

    m_device->call().vkDestroyPipelineLayout(m_device->get(),
                                             m_layout,
                                             memory::instance().alloc());
    m_layout = VK_NULL_HANDLE;

    clear();
}

//-----------------------------------------------------------------------------
void pipeline_layout::bind_descriptor_set(VkCommandBuffer cmd_buf,
                                          VkDescriptorSet descriptor_set,
                                          index first_set,
                                          offset_list offsets,
                                          VkPipelineBindPoint bind_point) {
    std::array<VkDescriptorSet, 1> const descriptor_sets = {descriptor_set};

    m_device->call().vkCmdBindDescriptorSets(cmd_buf,
                                             bind_point,
                                             m_layout,
                                             first_set,
                                             to_ui32(descriptor_sets.size()),
                                             descriptor_sets.data(),
                                             to_ui32(offsets.size()),
                                             offsets.data());
}

} // namespace lava
