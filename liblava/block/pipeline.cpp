// file      : liblava/block/pipeline.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#include <liblava/block/pipeline.hpp>

namespace lava {

    bool pipeline_layout::create(device_ptr d) {
        device = d;

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

        return check(device->call().vkCreatePipelineLayout(device->get(), &pipelineLayoutInfo, memory::alloc(), &layout));
    }

    void pipeline_layout::destroy() {
        if (!layout)
            return;

        device->call().vkDestroyPipelineLayout(device->get(), layout, memory::alloc());
        layout = VK_NULL_HANDLE;
    }

    void pipeline_layout::bind_descriptor_set(VkCommandBuffer cmd_buf, VkDescriptorSet descriptor_set, ui32 index, offset_list offsets, VkPipelineBindPoint bind_point) {
        std::array<VkDescriptorSet, 1> const descriptor_sets = { descriptor_set };

        device->call().vkCmdBindDescriptorSets(cmd_buf, bind_point, layout, index, to_ui32(descriptor_sets.size()), descriptor_sets.data(), to_ui32(offsets.size()), offsets.data());
    }

    pipeline::pipeline(device_ptr device_, VkPipelineCache pipeline_cache)
    : device(device_), pipeline_cache(pipeline_cache) {}

    pipeline::~pipeline() {
        pipeline_cache = VK_NULL_HANDLE;
        layout = nullptr;
    }

    bool pipeline::create() {
        return create_internal();
    }

    void pipeline::destroy() {
        destroy_internal();

        if (vk_pipeline) {
            device->call().vkDestroyPipeline(device->get(), vk_pipeline, memory::alloc());
            vk_pipeline = VK_NULL_HANDLE;
        }

        layout = nullptr;
    }

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

    pipeline::shader_stage::~shader_stage() {
        destroy();
    }

    void pipeline::shader_stage::add_specialization_entry(VkSpecializationMapEntry const& specialization) {
        specialization_entries.push_back(specialization);
        specialization_info.mapEntryCount = to_ui32(specialization_entries.size());
        specialization_info.pMapEntries = specialization_entries.data();
    }

    bool pipeline::shader_stage::create(device_ptr d, data const& shader_data, data const& specialization_data) {
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

    void pipeline::shader_stage::destroy() {
        if (!create_info.module)
            return;

        device->call().vkDestroyShaderModule(device->get(), create_info.module, memory::alloc());

        create_info.module = VK_NULL_HANDLE;
        device = nullptr;
    }

    pipeline::shader_stage::ptr make_pipeline_shader_stage(VkShaderStageFlagBits stage) {
        auto shaderStage = std::make_shared<pipeline::shader_stage>();

        shaderStage->set_stage(stage);
        return shaderStage;
    }

    pipeline::shader_stage::ptr create_pipeline_shader_stage(device_ptr device, data const& data, VkShaderStageFlagBits stage) {
        auto shaderStage = make_pipeline_shader_stage(stage);

        if (!shaderStage->create(device, data))
            return nullptr;

        return shaderStage;
    }

    graphics_pipeline::graphics_pipeline(device_ptr device_, VkPipelineCache pipeline_cache)
    : pipeline(device_, pipeline_cache) {
        info.vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info.vertex_input_state.pNext = nullptr;
        info.vertex_input_state.flags = 0;
        info.vertex_input_state.vertexBindingDescriptionCount = 0;
        info.vertex_input_state.pVertexBindingDescriptions = nullptr;
        info.vertex_input_state.vertexAttributeDescriptionCount = 0;
        info.vertex_input_state.pVertexAttributeDescriptions = nullptr;

        info.input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        info.input_assembly_state.pNext = nullptr;
        info.input_assembly_state.flags = 0;
        info.input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        info.input_assembly_state.primitiveRestartEnable = VK_FALSE;

        info.viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info.viewport_state.pNext = nullptr;
        info.viewport_state.flags = 0;
        info.viewport_state.viewportCount = 1;
        info.viewport_state.pViewports = nullptr;
        info.viewport_state.scissorCount = 1;
        info.viewport_state.pScissors = nullptr;

        info.multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info.multisample_state.pNext = nullptr;
        info.multisample_state.flags = 0;
        info.multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        info.multisample_state.sampleShadingEnable = VK_FALSE;
        info.multisample_state.minSampleShading = 0.f;
        info.multisample_state.pSampleMask = nullptr;
        info.multisample_state.alphaToCoverageEnable = VK_FALSE;
        info.multisample_state.alphaToOneEnable = VK_FALSE;

        info.depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        info.depth_stencil_state.pNext = nullptr;
        info.depth_stencil_state.flags = 0;
        info.depth_stencil_state.depthTestEnable = VK_FALSE;
        info.depth_stencil_state.depthWriteEnable = VK_FALSE;
        info.depth_stencil_state.depthCompareOp = VK_COMPARE_OP_NEVER;
        info.depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
        info.depth_stencil_state.stencilTestEnable = VK_FALSE;
        info.depth_stencil_state.front = {};
        info.depth_stencil_state.back = {};
        info.depth_stencil_state.minDepthBounds = 0.f;
        info.depth_stencil_state.maxDepthBounds = 0.f;

        info.rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        info.rasterization_state.pNext = nullptr;
        info.rasterization_state.flags = 0;
        info.rasterization_state.depthClampEnable = VK_FALSE;
        info.rasterization_state.rasterizerDiscardEnable = VK_FALSE;
        info.rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
        info.rasterization_state.cullMode = VK_CULL_MODE_NONE;
        info.rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        info.rasterization_state.depthBiasEnable = VK_FALSE;
        info.rasterization_state.depthBiasConstantFactor = 0.f;
        info.rasterization_state.depthBiasClamp = 0.f;
        info.rasterization_state.depthBiasSlopeFactor = 0.f;
        info.rasterization_state.lineWidth = 1.f;

        color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend_state.pNext = nullptr;
        color_blend_state.flags = 0;
        color_blend_state.logicOpEnable = VK_FALSE;
        color_blend_state.logicOp = VK_LOGIC_OP_CLEAR;
        color_blend_state.attachmentCount = 0;
        color_blend_state.pAttachments = nullptr;
        color_blend_state.blendConstants[0] = 0.f;
        color_blend_state.blendConstants[1] = 0.f;
        color_blend_state.blendConstants[2] = 0.f;
        color_blend_state.blendConstants[3] = 0.f;

        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.pNext = nullptr;
        dynamic_state.flags = 0;
        dynamic_state.dynamicStateCount = 0;
        dynamic_state.pDynamicStates = nullptr;

        set_dynamic_states({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
    }

    void graphics_pipeline::set_vertex_input_binding(VkVertexInputBindingDescription const& description) {
        VkVertexInputBindingDescriptions descriptions;
        descriptions.push_back(description);

        set_vertex_input_bindings(descriptions);
    }

    void graphics_pipeline::set_vertex_input_bindings(VkVertexInputBindingDescriptions const& descriptions) {
        vertex_input_bindings = descriptions;

        info.vertex_input_state.vertexBindingDescriptionCount = to_ui32(vertex_input_bindings.size());
        info.vertex_input_state.pVertexBindingDescriptions = vertex_input_bindings.data();
    }

    void graphics_pipeline::set_vertex_input_attribute(VkVertexInputAttributeDescription const& attribute) {
        VkVertexInputAttributeDescriptions attributes;
        attributes.push_back(attribute);

        set_vertex_input_attributes(attributes);
    }

    void graphics_pipeline::set_vertex_input_attributes(VkVertexInputAttributeDescriptions const& attributes) {
        vertex_input_attributes = attributes;

        info.vertex_input_state.vertexAttributeDescriptionCount = to_ui32(vertex_input_attributes.size());
        info.vertex_input_state.pVertexAttributeDescriptions = vertex_input_attributes.data();
    }

    void graphics_pipeline::set_depth_test_and_write(bool test_enable, bool write_enable) {
        info.depth_stencil_state.depthTestEnable = test_enable ? VK_TRUE : VK_FALSE;
        info.depth_stencil_state.depthWriteEnable = write_enable ? VK_TRUE : VK_FALSE;
    }

    void graphics_pipeline::set_depth_compare_op(VkCompareOp compare_op) {
        info.depth_stencil_state.depthCompareOp = compare_op;
    }

    void graphics_pipeline::set_rasterization_cull_mode(VkCullModeFlags cull_mode) {
        info.rasterization_state.cullMode = cull_mode;
    }

    void graphics_pipeline::set_rasterization_front_face(VkFrontFace front_face) {
        info.rasterization_state.frontFace = front_face;
    }

    void graphics_pipeline::set_rasterization_polygon_mode(VkPolygonMode polygon_mode) {
        info.rasterization_state.polygonMode = polygon_mode;
    }

    VkPipelineColorBlendAttachmentState graphics_pipeline::create_color_blend_attachment() {
        return {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
    }

    void graphics_pipeline::add_color_blend_attachment(VkPipelineColorBlendAttachmentState const& attachment) {
        color_blend_attachment_states.push_back(attachment);

        color_blend_state.attachmentCount = to_ui32(color_blend_attachment_states.size());
        color_blend_state.pAttachments = color_blend_attachment_states.data();
    }

    void graphics_pipeline::set_dynamic_states(VkDynamicStates const& states) {
        dynamic_states = states;

        dynamic_state.dynamicStateCount = to_ui32(dynamic_states.size());
        dynamic_state.pDynamicStates = dynamic_states.data();
    }

    void graphics_pipeline::add_dynamic_state(VkDynamicState state) {
        dynamic_states.push_back(state);
        set_dynamic_states(dynamic_states);
    }

    bool graphics_pipeline::add_shader_stage(data const& data, VkShaderStageFlagBits stage) {
        if (!data.ptr) {
            log()->error("graphics pipeline shader stage data");
            return false;
        }

        auto shader_stage = create_pipeline_shader_stage(device, data, stage);
        if (!shader_stage) {
            log()->error("create graphics pipeline shader stage");
            return false;
        }

        add(shader_stage);
        return true;
    }

    void graphics_pipeline::copy_to(graphics_pipeline* target) const {
        target->set_layout(layout);

        target->info = info;

        target->shader_stages = shader_stages;
        target->vertex_input_bindings = vertex_input_bindings;
        target->vertex_input_attributes = vertex_input_attributes;

        target->color_blend_attachment_states = color_blend_attachment_states;
        target->color_blend_state = color_blend_state;
        target->dynamic_states = dynamic_states;
    }

    bool graphics_pipeline::create_internal() {
        if (on_create) {
            if (!on_create(info))
                return false;
        }

        VkPipelineShaderStageCreateInfos stages;

        for (auto& shader_stage : shader_stages)
            stages.push_back(shader_stage->get_create_info());

        VkGraphicsPipelineCreateInfo const vk_create_info{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = to_ui32(stages.size()),
            .pStages = stages.data(),
            .pVertexInputState = &info.vertex_input_state,
            .pInputAssemblyState = &info.input_assembly_state,
            .pTessellationState = nullptr,
            .pViewportState = &info.viewport_state,
            .pRasterizationState = &info.rasterization_state,
            .pMultisampleState = &info.multisample_state,
            .pDepthStencilState = &info.depth_stencil_state,
            .pColorBlendState = &color_blend_state,
            .pDynamicState = &dynamic_state,
            .layout = layout->get(),
            .renderPass = render_pass,
            .subpass = to_ui32(subpass),
            .basePipelineHandle = 0,
            .basePipelineIndex = -1,
        };

        std::array<VkGraphicsPipelineCreateInfo, 1> const vk_info = { vk_create_info };

        return check(device->call().vkCreateGraphicsPipelines(device->get(), pipeline_cache,
                                                              to_ui32(vk_info.size()), vk_info.data(),
                                                              memory::alloc(), &vk_pipeline));
    }

    void graphics_pipeline::destroy_internal() {
        shader_stages.clear();
    }

    void graphics_pipeline::bind(VkCommandBuffer cmd_buf) {
        vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline);
    }

    void graphics_pipeline::set_viewport_and_scissor(VkCommandBuffer cmd_buf, uv2 size) {
        VkViewport viewportParam;
        viewportParam.x = 0.f;
        viewportParam.y = 0.f;
        viewportParam.width = to_r32(size.x);
        viewportParam.height = to_r32(size.y);
        viewportParam.minDepth = 0.f;
        viewportParam.maxDepth = 1.f;

        VkRect2D scissorParam;
        scissorParam.offset = { 0, 0 };
        scissorParam.extent = { size.x, size.y };

        if (size_type == size_type::absolute) {
            viewportParam = viewport;
            scissorParam = scissor;
        } else if (size_type == size_type::relative) {
            viewportParam.x = viewport.x * size.x;
            viewportParam.y = viewport.y * size.y;
            viewportParam.width = viewport.width * size.x;
            viewportParam.height = viewport.height * size.y;

            scissorParam.offset.x = scissor.offset.x * size.x;
            scissorParam.offset.y = scissor.offset.y * size.y;
            scissorParam.extent.width = scissor.extent.width * size.x;
            scissorParam.extent.height = scissor.extent.height * size.y;
        } else {
            viewport = viewportParam;
            scissor = scissorParam;
        }

        std::array<VkViewport, 1> const viewports = { viewportParam };
        vkCmdSetViewport(cmd_buf, 0, to_ui32(viewports.size()), viewports.data());

        std::array<VkRect2D, 1> const scissors = { scissorParam };
        vkCmdSetScissor(cmd_buf, 0, to_ui32(scissors.size()), scissors.data());
    }

    void compute_pipeline::bind(VkCommandBuffer cmd_buf) {
        vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, vk_pipeline);
    }

    bool compute_pipeline::set_shader_stage(data const& data, VkShaderStageFlagBits stage) {
        if (!data.ptr) {
            log()->error("compute pipeline shader stage data");
            return false;
        }

        auto shader_stage = create_pipeline_shader_stage(device, data, stage);
        if (!shader_stage) {
            log()->error("create compute pipeline shader stage");
            return false;
        }

        set(shader_stage);
        return true;
    }

    bool compute_pipeline::create_internal() {
        VkComputePipelineCreateInfo const create_info{
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .stage = shader_stage->get_create_info(),
            .layout = layout->get(),
            .basePipelineHandle = 0,
            .basePipelineIndex = -1,
        };

        std::array<VkComputePipelineCreateInfo, 1> const info = { create_info };

        return check(device->call().vkCreateComputePipelines(device->get(), pipeline_cache, to_ui32(info.size()),
                                                             info.data(), memory::alloc(), &vk_pipeline));
    }

    void compute_pipeline::destroy_internal() {
        shader_stage = nullptr;
    }

    void compute_pipeline::copy_to(compute_pipeline* target) const {
        target->set_layout(layout);

        target->shader_stage = shader_stage;
    }

} // namespace lava
