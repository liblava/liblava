/**
 * @file         liblava/block/render_pass.cpp
 * @brief        Render pass
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/block/render_pass.hpp>
#include <liblava/util/log.hpp>

namespace lava {

//-----------------------------------------------------------------------------
render_pass::render_pass(device_p d)
: device(d) {
    callback.on_created =
        [&](VkAttachmentsRef target_attachments, rect::ref area) { return on_target_created(
                                                                       target_attachments, area); };
    callback.on_destroyed = [&]() { on_target_destroyed(); };
}

//-----------------------------------------------------------------------------
bool render_pass::create(VkAttachmentsRef target_attachments,
                         rect::ref area) {
    std::vector<VkAttachmentDescription> attachment_descriptions;

    for (auto& attachment : attachments)
        attachment_descriptions.push_back(attachment->get_description());

    std::vector<VkSubpassDescription> subpass_descriptions;

    for (auto& subpass : subpasses)
        subpass_descriptions.push_back(subpass->get_description());

    std::vector<VkSubpassDependency> subpass_dependencies;

    for (auto& dependency : dependencies)
        subpass_dependencies.push_back(dependency->get_dependency());

    VkRenderPassCreateInfo const create_info{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = to_ui32(attachment_descriptions.size()),
        .pAttachments = attachment_descriptions.data(),
        .subpassCount = to_ui32(subpass_descriptions.size()),
        .pSubpasses = subpass_descriptions.data(),
        .dependencyCount = to_ui32(subpass_dependencies.size()),
        .pDependencies = subpass_dependencies.data(),
    };

    if (!check(device->call().vkCreateRenderPass(device->get(),
                                                 &create_info,
                                                 memory::instance().alloc(),
                                                 &vk_render_pass))) {
        log()->error("create render pass");
        return false;
    }

    return on_target_created(target_attachments, area);
}

//-----------------------------------------------------------------------------
void render_pass::destroy() {
    attachments.clear();
    dependencies.clear();

    for (auto& subpass : subpasses)
        subpass->destroy();

    subpasses.clear();

    on_target_destroyed();

    if (vk_render_pass) {
        device->call().vkDestroyRenderPass(device->get(),
                                           vk_render_pass,
                                           memory::instance().alloc());
        vk_render_pass = VK_NULL_HANDLE;
    }

    device = nullptr;
}

//-----------------------------------------------------------------------------
void render_pass::begin(VkCommandBuffer cmd_buf,
                        index frame) {
    auto origin = area.get_origin();
    auto size = area.get_size();

    VkRenderPassBeginInfo const info{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = vk_render_pass,
        .framebuffer = framebuffers[frame],
        .renderArea = { { origin.x, origin.y }, { size.x, size.y } },
        .clearValueCount = to_ui32(clear_values.size()),
        .pClearValues = clear_values.data(),
    };

    device->call().vkCmdBeginRenderPass(cmd_buf,
                                        &info,
                                        VK_SUBPASS_CONTENTS_INLINE);
}

//-----------------------------------------------------------------------------
void render_pass::end(VkCommandBuffer cmd_buf) {
    device->call().vkCmdEndRenderPass(cmd_buf);
}

//-----------------------------------------------------------------------------
void render_pass::process(VkCommandBuffer cmd_buf,
                          index frame) {
    begin(cmd_buf, frame);

    ui32 count = 0;

    for (auto& subpass : subpasses) {
        if (count > 0)
            device->call().vkCmdNextSubpass(cmd_buf,
                                            VK_SUBPASS_CONTENTS_INLINE);

        if (!subpass->activated())
            continue;

        subpass->process(cmd_buf, area.get_size());

        ++count;
    }

    end(cmd_buf);
}

//-----------------------------------------------------------------------------
void render_pass::set_clear_color(v3 value) {
    clear_values.resize(2);

    clear_values[0].color.float32[0] = value.r;
    clear_values[0].color.float32[1] = value.g;
    clear_values[0].color.float32[2] = value.b;
    clear_values[0].color.float32[3] = 1.f;

    clear_values[1].depthStencil = { 1.f, 0 };
}

//-----------------------------------------------------------------------------
v3 render_pass::get_clear_color() const {
    return {
        clear_values[0].color.float32[0],
        clear_values[0].color.float32[1],
        clear_values[0].color.float32[2],
    };
}

//-----------------------------------------------------------------------------
bool render_pass::on_target_created(VkAttachmentsRef target_attachments,
                                    rect::ref a) {
    area = a;
    framebuffers.resize(target_attachments.size());

    auto size = area.get_size();

    ui32 count = 0;

    for (auto& attachment : target_attachments) {
        VkFramebufferCreateInfo const create_info{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = vk_render_pass,
            .attachmentCount = to_ui32(attachment.size()),
            .pAttachments = attachment.data(),
            .width = size.x,
            .height = size.y,
            .layers = 1,
        };

        if (failed(device->call().vkCreateFramebuffer(device->get(),
                                                      &create_info,
                                                      memory::instance().alloc(),
                                                      &framebuffers[count]))) {
            log()->error("create render pass target");
            return false;
        }

        ++count;
    }

    return true;
}

//-----------------------------------------------------------------------------
void render_pass::on_target_destroyed() {
    for (auto& framebuffer : framebuffers) {
        if (!framebuffer)
            continue;

        device->call().vkDestroyFramebuffer(device->get(),
                                            framebuffer,
                                            memory::instance().alloc());
        framebuffer = VK_NULL_HANDLE;
    }

    framebuffers.clear();
}

} // namespace lava
