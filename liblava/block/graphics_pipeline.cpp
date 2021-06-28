/**
 * @file         liblava/block/graphics_pipeline.cpp
 * @brief        Graphics pipeline
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/block/graphics_pipeline.hpp>

namespace lava {

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
void graphics_pipeline::set_vertex_input_binding(VkVertexInputBindingDescription const& description) {
    VkVertexInputBindingDescriptions descriptions;
    descriptions.push_back(description);

    set_vertex_input_bindings(descriptions);
}

//-----------------------------------------------------------------------------
void graphics_pipeline::set_vertex_input_bindings(VkVertexInputBindingDescriptions const& descriptions) {
    vertex_input_bindings = descriptions;

    info.vertex_input_state.vertexBindingDescriptionCount = to_ui32(vertex_input_bindings.size());
    info.vertex_input_state.pVertexBindingDescriptions = vertex_input_bindings.data();
}

//-----------------------------------------------------------------------------
void graphics_pipeline::set_vertex_input_attribute(VkVertexInputAttributeDescription const& attribute) {
    VkVertexInputAttributeDescriptions attributes;
    attributes.push_back(attribute);

    set_vertex_input_attributes(attributes);
}

//-----------------------------------------------------------------------------
void graphics_pipeline::set_vertex_input_attributes(VkVertexInputAttributeDescriptions const& attributes) {
    vertex_input_attributes = attributes;

    info.vertex_input_state.vertexAttributeDescriptionCount = to_ui32(vertex_input_attributes.size());
    info.vertex_input_state.pVertexAttributeDescriptions = vertex_input_attributes.data();
}

//-----------------------------------------------------------------------------
void graphics_pipeline::set_input_topology(VkPrimitiveTopology const& topology) {
    info.input_assembly_state.topology = topology;
}

//-----------------------------------------------------------------------------
void graphics_pipeline::set_depth_test_and_write(bool enable_test, bool enable_write) {
    info.depth_stencil_state.depthTestEnable = enable_test ? VK_TRUE : VK_FALSE;
    info.depth_stencil_state.depthWriteEnable = enable_write ? VK_TRUE : VK_FALSE;
}

//-----------------------------------------------------------------------------
void graphics_pipeline::set_depth_compare_op(VkCompareOp compare_op) {
    info.depth_stencil_state.depthCompareOp = compare_op;
}

//-----------------------------------------------------------------------------
void graphics_pipeline::set_rasterization_cull_mode(VkCullModeFlags cull_mode) {
    info.rasterization_state.cullMode = cull_mode;
}

//-----------------------------------------------------------------------------
void graphics_pipeline::set_rasterization_front_face(VkFrontFace front_face) {
    info.rasterization_state.frontFace = front_face;
}

//-----------------------------------------------------------------------------
void graphics_pipeline::set_rasterization_polygon_mode(VkPolygonMode polygon_mode) {
    info.rasterization_state.polygonMode = polygon_mode;
}

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
void graphics_pipeline::add_color_blend_attachment(VkPipelineColorBlendAttachmentState const& attachment) {
    color_blend_attachment_states.push_back(attachment);

    color_blend_state.attachmentCount = to_ui32(color_blend_attachment_states.size());
    color_blend_state.pAttachments = color_blend_attachment_states.data();
}

//-----------------------------------------------------------------------------
void graphics_pipeline::clear_color_blend_attachment() {
    color_blend_attachment_states.clear();

    color_blend_state.attachmentCount = 0;
    color_blend_state.pAttachments = nullptr;
}

//-----------------------------------------------------------------------------
void graphics_pipeline::set_dynamic_states(VkDynamicStates const& states) {
    dynamic_states = states;

    dynamic_state.dynamicStateCount = to_ui32(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();
}

//-----------------------------------------------------------------------------
void graphics_pipeline::add_dynamic_state(VkDynamicState state) {
    dynamic_states.push_back(state);
    set_dynamic_states(dynamic_states);
}

//-----------------------------------------------------------------------------
void graphics_pipeline::clear_dynamic_states() {
    dynamic_states.clear();

    dynamic_state.dynamicStateCount = 0;
    dynamic_state.pDynamicStates = nullptr;
}

//-----------------------------------------------------------------------------
bool graphics_pipeline::add_shader_stage(cdata const& data, VkShaderStageFlagBits stage) {
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

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
void graphics_pipeline::destroy_internal() {
    clear();
}

//-----------------------------------------------------------------------------
void graphics_pipeline::bind(VkCommandBuffer cmd_buf) {
    vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline);
}

//-----------------------------------------------------------------------------
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

    if (sizing_mode == sizing_mode::absolute) {
        viewportParam = viewport;
        scissorParam = scissor;
    } else if (sizing_mode == sizing_mode::relative) {
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

} // namespace lava
