// file      : liblava/block/pipeline.cpp
// authors   : Lava Block OÜ and contributors
// copyright : Copyright (c) 2018-present, MIT License

#include <liblava/block/pipeline.hpp>

namespace lava {

//-----------------------------------------------------------------------------
pipeline::pipeline(device_ptr device_, VkPipelineCache pipeline_cache)
: device(device_), pipeline_cache(pipeline_cache) {}

//-----------------------------------------------------------------------------
pipeline::~pipeline() {
    pipeline_cache = VK_NULL_HANDLE;
    layout = nullptr;
}

//-----------------------------------------------------------------------------
bool pipeline::create() {
    return create_internal();
}

//-----------------------------------------------------------------------------
void pipeline::destroy() {
    destroy_internal();

    if (vk_pipeline) {
        device->call().vkDestroyPipeline(device->get(), vk_pipeline, memory::alloc());
        vk_pipeline = VK_NULL_HANDLE;
    }

    layout = nullptr;
}

//-----------------------------------------------------------------------------
pipeline::shader_stage::shader_stage() {
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    create_info.module = VK_NULL_HANDLE;
    create_info.pName = _main_;
    create_info.pSpecializationInfo = &specialization_info;

    specialization_info.mapEntryCount = 0;
    specialization_info.pMapEntries = nullptr;
    specialization_info.dataSize = 0;
    specialization_info.pData = nullptr;
}

//-----------------------------------------------------------------------------
pipeline::shader_stage::~shader_stage() {
    destroy();
}

//-----------------------------------------------------------------------------
void pipeline::shader_stage::add_specialization_entry(VkSpecializationMapEntry const& specialization) {
    specialization_entries.push_back(specialization);
    specialization_info.mapEntryCount = to_ui32(specialization_entries.size());
    specialization_info.pMapEntries = specialization_entries.data();
}

//-----------------------------------------------------------------------------
bool pipeline::shader_stage::create(device_ptr d, cdata const& shader_data, cdata const& specialization_data) {
    device = d;

    if (specialization_data.size > 0) {
        specialization_data_copy.free();
        specialization_data_copy.set(specialization_data.size);
        memcpy(specialization_data_copy.ptr, specialization_data.ptr, specialization_data.size);

        specialization_info.dataSize = specialization_data_copy.size;
        specialization_info.pData = specialization_data_copy.ptr;
    }

    create_info.module = create_shader_module(device, shader_data);

    return create_info.module != VK_NULL_HANDLE;
}

//-----------------------------------------------------------------------------
void pipeline::shader_stage::destroy() {
    if (!create_info.module)
        return;

    device->call().vkDestroyShaderModule(device->get(), create_info.module, memory::alloc());

    create_info.module = VK_NULL_HANDLE;
    device = nullptr;
}

//-----------------------------------------------------------------------------
pipeline::shader_stage::ptr make_pipeline_shader_stage(VkShaderStageFlagBits stage) {
    auto shaderStage = std::make_shared<pipeline::shader_stage>();

    shaderStage->set_stage(stage);
    return shaderStage;
}

//-----------------------------------------------------------------------------
pipeline::shader_stage::ptr create_pipeline_shader_stage(device_ptr device, cdata const& data, VkShaderStageFlagBits stage) {
    auto shaderStage = make_pipeline_shader_stage(stage);

    if (!shaderStage->create(device, data))
        return nullptr;

    return shaderStage;
}

} // namespace lava
