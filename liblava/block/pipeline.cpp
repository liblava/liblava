// file      : liblava/block/pipeline.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/block/pipeline.hpp>

namespace lava {

bool pipeline_layout::create(device* device) {

	dev = device;

	VkDescriptorSetLayouts layouts;
	for (auto& layout : descriptors)
		layouts.push_back(layout->get());

    VkPipelineLayoutCreateInfo pipelineLayoutInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = to_ui32(layouts.size()),
        .pSetLayouts = layouts.data(),
        .pushConstantRangeCount = to_ui32(push_constant_ranges.size()),
        .pPushConstantRanges = push_constant_ranges.data(),
    };
	
	return check(dev->call().vkCreatePipelineLayout(dev->get(), &pipelineLayoutInfo, memory::alloc(), &layout));
}

void pipeline_layout::destroy() {

	if (!layout)
		return;

	dev->call().vkDestroyPipelineLayout(dev->get(), layout, memory::alloc());
	layout = nullptr;
}

void pipeline_layout::bind_descriptor_set(VkCommandBuffer cmd_buf, VkDescriptorSet descriptor_set, offset_list offsets) {

	std::array<VkDescriptorSet, 1> const descriptor_sets = { descriptor_set };

	vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 
                            to_ui32(descriptor_sets.size()), descriptor_sets.data(), to_ui32(offsets.size()), offsets.data());
}

pipeline::pipeline(device* device, VkPipelineCache pipeline_cache) : dev(device), pipeline_cache(pipeline_cache) {}

pipeline::~pipeline() {

	pipeline_cache = nullptr;
	layout = nullptr;
}

bool pipeline::create() { return create_internal(); }

void pipeline::destroy() {

	destroy_internal();

	if (_pipeline) {

		dev->call().vkDestroyPipeline(dev->get(), _pipeline, memory::alloc());
		_pipeline = nullptr;
	}

	layout = nullptr;
}

pipeline::shader_stage::shader_stage() {

	create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    create_info.module = nullptr;
    create_info.pName = _main_;
    create_info.pSpecializationInfo = nullptr;
}

pipeline::shader_stage::~shader_stage() {

	destroy();
}

pipeline::shader_stage::ptr pipeline::shader_stage::create(VkShaderStageFlagBits stage) {

	auto shaderStage = std::make_shared<shader_stage>();

	shaderStage->set_stage(stage);
	return shaderStage;
}

pipeline::shader_stage::ptr pipeline::shader_stage::create(device* device, data const& data, VkShaderStageFlagBits stage) {

	auto shaderStage = create(stage);

	if (!shaderStage->create(device, data))
		return nullptr;

	return shaderStage;
}

bool pipeline::shader_stage::create(device* device, data const& data) {

	dev = device;

    create_info.module = create_shader_module(dev, data);

    return create_info.module != nullptr;
}

void pipeline::shader_stage::destroy() {

	if (!create_info.module)
		return;

	dev->call().vkDestroyShaderModule(dev->get(), create_info.module, memory::alloc());

    create_info.module = nullptr;
	dev = nullptr;
}

graphics_pipeline::graphics_pipeline(device* device, VkPipelineCache pipeline_cache) : pipeline(device, pipeline_cache) {

    vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state.pNext = nullptr;
    vertex_input_state.flags = 0;
    vertex_input_state.vertexBindingDescriptionCount = 0;
    vertex_input_state.pVertexBindingDescriptions = nullptr;
    vertex_input_state.vertexAttributeDescriptionCount = 0;
    vertex_input_state.pVertexAttributeDescriptions = nullptr;

	input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state.pNext = nullptr;
    input_assembly_state.flags = 0;
    input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_state.primitiveRestartEnable = VK_FALSE;

	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.pNext = nullptr;
    viewport_state.flags = 0;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = nullptr;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = nullptr;

	multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state.pNext = nullptr;
    multisample_state.flags = 0;
    multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_state.sampleShadingEnable = VK_FALSE;
    multisample_state.minSampleShading = 0.f;
    multisample_state.pSampleMask = nullptr;
    multisample_state.alphaToCoverageEnable = VK_FALSE;
    multisample_state.alphaToOneEnable = VK_FALSE;

	depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_state.pNext = nullptr;
    depth_stencil_state.flags = 0;
    depth_stencil_state.depthTestEnable = VK_FALSE;
    depth_stencil_state.depthWriteEnable = VK_FALSE;
    depth_stencil_state.depthCompareOp = VK_COMPARE_OP_NEVER;
    depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state.stencilTestEnable = VK_FALSE;
    depth_stencil_state.front = {};
    depth_stencil_state.back = {};
    depth_stencil_state.minDepthBounds = 0.f;
    depth_stencil_state.maxDepthBounds = 0.f;

	rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state.pNext = nullptr;
    rasterization_state.flags = 0;
    rasterization_state.depthClampEnable = VK_FALSE;
    rasterization_state.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state.cullMode = VK_CULL_MODE_NONE;
    rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state.depthBiasEnable = VK_FALSE;
    rasterization_state.depthBiasConstantFactor = 0.f;
    rasterization_state.depthBiasClamp = 0.f;
    rasterization_state.depthBiasSlopeFactor = 0.f;
    rasterization_state.lineWidth = 1.f;

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

	vertex_input_state.vertexBindingDescriptionCount = to_ui32(vertex_input_bindings.size());
	vertex_input_state.pVertexBindingDescriptions = vertex_input_bindings.data();
}

void graphics_pipeline::set_vertex_input_attribute(VkVertexInputAttributeDescription const& attribute) {

	VkVertexInputAttributeDescriptions attributes;
	attributes.push_back(attribute);

	set_vertex_input_attributes(attributes);
}

void graphics_pipeline::set_vertex_input_attributes(VkVertexInputAttributeDescriptions const& attributes) {

	vertex_input_attributes = attributes;

	vertex_input_state.vertexAttributeDescriptionCount = to_ui32(vertex_input_attributes.size());
	vertex_input_state.pVertexAttributeDescriptions = vertex_input_attributes.data();
}

void graphics_pipeline::set_depth_test_and_write(bool test_enable, bool write_enable) {

	depth_stencil_state.depthTestEnable = test_enable ? VK_TRUE : VK_FALSE;
	depth_stencil_state.depthWriteEnable = write_enable ? VK_TRUE : VK_FALSE;
}

void graphics_pipeline::set_depth_compare_op(VkCompareOp compare_op) {

	depth_stencil_state.depthCompareOp = compare_op;
}

void graphics_pipeline::set_rasterization_cull_mode(VkCullModeFlags cull_mode) {

	rasterization_state.cullMode = cull_mode;
}

void graphics_pipeline::set_rasterization_front_face(VkFrontFace front_face) {

	rasterization_state.frontFace = front_face;
}

void graphics_pipeline::set_rasterization_polygon_mode(VkPolygonMode polygon_mode) {

	rasterization_state.polygonMode = polygon_mode;
}

VkPipelineColorBlendAttachmentState graphics_pipeline::create_color_blend_attachment() {

    return
    {
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

		log()->error("graphics_pipeline::add_shader_stage data ptr failed");
		return false;
	}

	auto shader_stage = pipeline::shader_stage::create(dev, data, stage);
	if (!shader_stage) {

		log()->error("graphics_pipeline::add_shader_stage shader_stage::create failed");
		return false;
	}

	add(shader_stage);
	return true;
}

bool graphics_pipeline::add_shader_stage(name filename, VkShaderStageFlagBits stage) {

    return add_shader_stage(file_data(filename).get(), stage);
}

void graphics_pipeline::copy_to(graphics_pipeline* target) const {

	target->set_layout(layout);

	target->shader_stages = shader_stages;
	target->vertex_input_state = vertex_input_state;
	target->vertex_input_bindings = vertex_input_bindings;
	target->vertex_input_attributes = vertex_input_attributes;
	target->input_assembly_state = input_assembly_state;
	target->viewport_state = viewport_state;
	target->multisample_state = multisample_state;
	target->depth_stencil_state = depth_stencil_state;
	target->rasterization_state = rasterization_state;
	target->color_blend_state = color_blend_state;
	target->color_blend_attachment_states = color_blend_attachment_states;
	target->dynamic_state = dynamic_state;
	target->dynamic_states = dynamic_states;
}

bool graphics_pipeline::create_internal() {

	VkPipelineShaderStageCreateInfos stages;

	for (auto& shader_stage : shader_stages)
		stages.push_back(shader_stage->get_create_info());

	VkGraphicsPipelineCreateInfo create_info
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = to_ui32(stages.size()),
        .pStages = stages.data(),
        .pVertexInputState = &vertex_input_state,
        .pInputAssemblyState = &input_assembly_state,
        .pTessellationState = nullptr,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterization_state,
        .pMultisampleState = &multisample_state,
        .pDepthStencilState = &depth_stencil_state,
        .pColorBlendState = &color_blend_state,
        .pDynamicState = &dynamic_state,
        .layout = layout->get(),
        .renderPass = render_pass,
        .subpass = 0,
        .basePipelineHandle = nullptr,
        .basePipelineIndex = -1,
    };

	std::array<VkGraphicsPipelineCreateInfo, 1> const create_infos = { create_info };

	return check(dev->call().vkCreateGraphicsPipelines(dev->get(), pipeline_cache, 
                                                        to_ui32(create_infos.size()), create_infos.data(), memory::alloc(), &_pipeline));
}

void graphics_pipeline::destroy_internal() { shader_stages.clear(); }

void graphics_pipeline::bind(VkCommandBuffer cmd_buf) {

	vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
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

	if (size_type._value == graphics_pipeline_size_type::absolute) {

		viewportParam = viewport;
		scissorParam = scissor;

	} else if (size_type._value == graphics_pipeline_size_type::relative) {

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

	vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, _pipeline);
}

bool compute_pipeline::set_shader_stage(data const& data, VkShaderStageFlagBits stage) {

    if (!data.ptr) {

        log()->error("compute_pipeline::add_shader_stage data ptr failed");
        return false;
    }

	auto shader_stage = pipeline::shader_stage::create(dev, data, stage);
	if (!shader_stage) {

		log()->error("compute_pipeline::set_shader_stage shader_stage::create failed");
		return false;
	}

	set(shader_stage);
	return true;
}

bool compute_pipeline::set_shader_stage(name filename, VkShaderStageFlagBits stage) {

    return set_shader_stage(file_data(filename).get(), stage);
}

bool compute_pipeline::create_internal() {

    VkComputePipelineCreateInfo create_info
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stage = shader_stage->get_create_info(),
        .layout = layout->get(),
        .basePipelineHandle = nullptr,
        .basePipelineIndex = -1,
    };

	std::array<VkComputePipelineCreateInfo, 1> const create_infos = { create_info };

	return check(dev->call().vkCreateComputePipelines(dev->get(), pipeline_cache, to_ui32(create_infos.size()), 
                                                        create_infos.data(), memory::alloc(), &_pipeline));
}

void compute_pipeline::destroy_internal() { shader_stage = nullptr; }

void compute_pipeline::copy_to(compute_pipeline* target) const {

	target->set_layout(layout);

	target->shader_stage = shader_stage;
}

} // lava
