/**
 * @file         liblava/block/compute_pipeline.cpp
 * @brief        Compute pipeline
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/block/compute_pipeline.hpp"
#include "liblava/util/log.hpp"

namespace lava {

//-----------------------------------------------------------------------------
void compute_pipeline::bind(VkCommandBuffer cmd_buf) {
    vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, m_vk_pipeline);
}

//-----------------------------------------------------------------------------
bool compute_pipeline::set_shader_stage(c_data::ref data,
                                        VkShaderStageFlagBits stage) {
    if (!data.addr) {
        logger()->error("compute pipeline shader stage data");
        return false;
    }

    auto shader_stage = create_pipeline_shader_stage(m_device, data, stage);
    if (!m_shader_stage) {
        logger()->error("create compute pipeline shader stage");
        return false;
    }

    set(m_shader_stage);
    return true;
}

//-----------------------------------------------------------------------------
bool compute_pipeline::setup() {
    VkComputePipelineCreateInfo const create_info{
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = m_shader_stage->get_create_info(),
        .layout = m_layout->get(),
        .basePipelineHandle = 0,
        .basePipelineIndex = undef,
    };

    std::array<VkComputePipelineCreateInfo, 1> const info = {create_info};

    return check(m_device->call().vkCreateComputePipelines(m_device->get(),
                                                           m_pipeline_cache,
                                                           to_ui32(info.size()),
                                                           info.data(),
                                                           memory::instance().alloc(),
                                                           &m_vk_pipeline));
}

//-----------------------------------------------------------------------------
void compute_pipeline::teardown() {
    m_shader_stage = nullptr;
}

//-----------------------------------------------------------------------------
void compute_pipeline::copy_to(compute_pipeline* target) const {
    target->set_layout(m_layout);

    target->m_shader_stage = m_shader_stage;
}

} // namespace lava
