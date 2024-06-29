/**
 * @file         liblava/block/pipeline.cpp
 * @brief        Pipeline
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/block/pipeline.hpp"
#include "liblava/block/def.hpp"

namespace lava {

//-----------------------------------------------------------------------------
pipeline::pipeline(device::ptr dev,
                   VkPipelineCache pipeline_cache)
: m_device(dev), m_pipeline_cache(pipeline_cache) {}

//-----------------------------------------------------------------------------
pipeline::~pipeline() {
    m_pipeline_cache = VK_NULL_HANDLE;
    m_layout = nullptr;
}

//-----------------------------------------------------------------------------
bool pipeline::create() {
    return setup();
}

//-----------------------------------------------------------------------------
void pipeline::destroy() {
    teardown();

    if (m_vk_pipeline) {
        m_device->call().vkDestroyPipeline(m_device->get(),
                                           m_vk_pipeline,
                                           memory::instance().alloc());
        m_vk_pipeline = VK_NULL_HANDLE;
    }

    m_layout = nullptr;
}

//-----------------------------------------------------------------------------
pipeline::shader_stage::shader_stage() {
    m_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_create_info.pNext = nullptr;
    m_create_info.flags = 0;
    m_create_info.stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    m_create_info.module = VK_NULL_HANDLE;
    m_create_info.pName = _main_;
    m_create_info.pSpecializationInfo = &m_specialization_info;

    m_specialization_info.mapEntryCount = 0;
    m_specialization_info.pMapEntries = nullptr;
    m_specialization_info.dataSize = 0;
    m_specialization_info.pData = nullptr;
}

//-----------------------------------------------------------------------------
pipeline::shader_stage::~shader_stage() {
    destroy();
}

//-----------------------------------------------------------------------------
void pipeline::shader_stage::add_specialization_entry(
    VkSpecializationMapEntry const& specialization) {
    m_specialization_entries.push_back(specialization);
    m_specialization_info.mapEntryCount = to_ui32(m_specialization_entries.size());
    m_specialization_info.pMapEntries = m_specialization_entries.data();
}

//-----------------------------------------------------------------------------
bool pipeline::shader_stage::create(device::ptr dev,
                                    c_data::ref shader_data,
                                    c_data::ref specialization_data) {
    m_device = dev;

    if (specialization_data.size > 0) {
        m_specialization_data_copy.deallocate();
        m_specialization_data_copy.set(specialization_data.size);
        memcpy(m_specialization_data_copy.addr,
               specialization_data.addr,
               specialization_data.size);

        m_specialization_info.dataSize = m_specialization_data_copy.size;
        m_specialization_info.pData = m_specialization_data_copy.addr;
    }

    m_create_info.module = create_shader_module(m_device, shader_data);

    return m_create_info.module != VK_NULL_HANDLE;
}

//-----------------------------------------------------------------------------
void pipeline::shader_stage::destroy() {
    if (!m_create_info.module)
        return;

    m_device->call().vkDestroyShaderModule(m_device->get(),
                                           m_create_info.module,
                                           memory::instance().alloc());

    m_create_info.module = VK_NULL_HANDLE;
    m_device = nullptr;
}

//-----------------------------------------------------------------------------
pipeline::shader_stage::s_ptr create_pipeline_shader_stage(device::ptr device,
                                                           c_data::ref data,
                                                           VkShaderStageFlagBits stage) {
    auto shaderStage = pipeline::shader_stage::make(stage);

    if (!shaderStage->create(device, data))
        return nullptr;

    return shaderStage;
}

} // namespace lava
