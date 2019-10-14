// file      : liblava/block/render_pass.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/block/render_pass.hpp>

namespace lava {

render_pass::render_pass(device* device) : dev(device) {

    on_created = [&](VkAttachmentsRef target_attachments, rect area) { return on_target_created(target_attachments, area); };
    on_destroyed = [&]() { on_target_destroyed(); };
}

bool render_pass::create(VkAttachmentsRef target_attachments, rect area) {

	std::vector<VkAttachmentDescription> attachment_descriptions;

	for (auto& attachment : attachments)
        attachment_descriptions.push_back(attachment->get_description());

	std::vector<VkSubpassDescription> subpass_descriptions;

	for (auto& subpass : subpasses)
        subpass_descriptions.push_back(subpass->get_description());

	std::vector<VkSubpassDependency> subpass_dependencies;

	for (auto& dependency : dependencies)
        subpass_dependencies.push_back(dependency->get_dependency());

    VkRenderPassCreateInfo create_info
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = to_ui32(attachment_descriptions.size()),
        .pAttachments = attachment_descriptions.data(),
        .subpassCount = to_ui32(subpass_descriptions.size()),
        .pSubpasses = subpass_descriptions.data(),
        .dependencyCount = to_ui32(subpass_dependencies.size()),
        .pDependencies = subpass_dependencies.data(),
    };

	if (!check(dev->call().vkCreateRenderPass(dev->get(), &create_info, memory::alloc(), &_render_pass))) {

		log()->error("render_pass::create vkCreateRenderPass failed");
		return false;
	}

	return on_target_created(target_attachments, area);
}

void render_pass::destroy() {

	attachments.clear();
	dependencies.clear();

	for (auto& subpass : subpasses)
		subpass->destroy();

	subpasses.clear();

	on_target_destroyed();

	if (_render_pass) {

		dev->call().vkDestroyRenderPass(dev->get(), _render_pass, memory::alloc());
        _render_pass = nullptr;
	}

	dev = nullptr;
}

void render_pass::begin(VkCommandBuffer cmd_buf, index frame) {

    auto origin = area.get_origin();
    auto size = area.get_size();

    VkRenderPassBeginInfo info
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = _render_pass,
        .framebuffer = framebuffers[frame],
        .renderArea = { { origin.x, origin.y }, { size.x, size.y } },
        .clearValueCount = to_ui32(clear_values.size()),
        .pClearValues = clear_values.data(),
    };

	dev->call().vkCmdBeginRenderPass(cmd_buf, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void render_pass::end(VkCommandBuffer cmd_buf) {

	dev->call().vkCmdEndRenderPass(cmd_buf);
}

void render_pass::process(VkCommandBuffer cmd_buf, index frame) {

	begin(cmd_buf, frame);

	ui32 count = 0;

	for (auto& subpass : subpasses) {

		if (count > 0)
			dev->call().vkCmdNextSubpass(cmd_buf, VK_SUBPASS_CONTENTS_INLINE);

		if (!subpass->is_active())
			continue;

		subpass->process(cmd_buf, area.get_size());

		++count;
	}

	end(cmd_buf);
}

void render_pass::set_clear_color(v3 value) {

    clear_values.resize(2);

    clear_values[0].color.float32[0] = value.r;
    clear_values[0].color.float32[1] = value.g;
    clear_values[0].color.float32[2] = value.b;
    clear_values[0].color.float32[3] = 1.f;
    
    clear_values[1].depthStencil = { 1.f, 0 };
}

bool render_pass::on_target_created(VkAttachmentsRef target_attachments, rect area_) {

	area = area_;
	framebuffers.resize(target_attachments.size());

    auto size = area.get_size();

	ui32 count = 0;

	for (auto& attachment : target_attachments) {

		VkFramebufferCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = _render_pass,
            .attachmentCount = to_ui32(attachment.size()),
            .pAttachments = attachment.data(),
            .width = size.x,
            .height = size.y,
            .layers = 1,
        };

		if (failed(dev->call().vkCreateFramebuffer(dev->get(), &create_info, memory::alloc(), &framebuffers[count]))) {

			log()->error("RenderPass::on_target_created vkCreateFramebuffer failed");
			return false;
		}

		++count;
	}

	return true;
}

void render_pass::on_target_destroyed() {

	for (auto& framebuffer : framebuffers) {

		if (!framebuffer)
			continue;

		dev->call().vkDestroyFramebuffer(dev->get(), framebuffer, memory::alloc());
		framebuffer = nullptr;
	}

	framebuffers.clear();
}

} // lava
