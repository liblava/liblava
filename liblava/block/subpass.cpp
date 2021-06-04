// file      : liblava/block/subpass.cpp
// authors   : Lava Block OÜ and contributors
// copyright : Copyright (c) 2018-present, MIT License

#include <liblava/block/subpass.hpp>

namespace lava {

//-----------------------------------------------------------------------------
subpass::subpass() {
    description.flags = 0;
    description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    description.inputAttachmentCount = 0;
    description.pInputAttachments = nullptr;
    description.colorAttachmentCount = 0;
    description.pColorAttachments = nullptr;
    description.pResolveAttachments = nullptr;
    description.pDepthStencilAttachment = nullptr;
    description.preserveAttachmentCount = 0;
    description.pPreserveAttachments = nullptr;
}

//-----------------------------------------------------------------------------
void subpass::destroy() {
    clear_pipelines();
}

//-----------------------------------------------------------------------------
void subpass::clear_pipelines() {
    for (auto& pipeline : pipelines)
        pipeline->destroy();

    pipelines.clear();
}

//-----------------------------------------------------------------------------
void subpass::remove(graphics_pipeline::ptr pipeline) {
    lava::remove(pipelines, std::move(pipeline));
}

//-----------------------------------------------------------------------------
void subpass::set_color_attachment(index attachment, VkImageLayout layout) {
    VkAttachmentReference reference;
    reference.attachment = attachment;
    reference.layout = layout;

    set_color_attachment(reference);
}

//-----------------------------------------------------------------------------
void subpass::set_color_attachment(VkAttachmentReference attachment) {
    VkAttachmentReferences attachments;
    attachments.push_back(attachment);

    set_color_attachments(attachments);
}

//-----------------------------------------------------------------------------
void subpass::set_color_attachments(VkAttachmentReferences const& attachments) {
    color_attachments = attachments;

    description.colorAttachmentCount = to_ui32(color_attachments.size());
    description.pColorAttachments = color_attachments.data();
}

//-----------------------------------------------------------------------------
void subpass::set_depth_stencil_attachment(index attachment, VkImageLayout layout) {
    VkAttachmentReference reference;
    reference.attachment = attachment;
    reference.layout = layout;

    set_depth_stencil_attachment(reference);
}

//-----------------------------------------------------------------------------
void subpass::set_depth_stencil_attachment(VkAttachmentReference attachment) {
    depth_stencil_attachment = attachment;

    description.pDepthStencilAttachment = &depth_stencil_attachment;
}

//-----------------------------------------------------------------------------
void subpass::set_input_attachment(index attachment, VkImageLayout layout) {
    VkAttachmentReference reference;
    reference.attachment = attachment;
    reference.layout = layout;

    set_input_attachment(reference);
}

//-----------------------------------------------------------------------------
void subpass::set_input_attachment(VkAttachmentReference attachment) {
    VkAttachmentReferences attachments;
    attachments.push_back(attachment);

    set_input_attachments(attachments);
}

//-----------------------------------------------------------------------------
void subpass::set_input_attachments(VkAttachmentReferences const& attachments) {
    input_attachments = attachments;

    description.inputAttachmentCount = to_ui32(input_attachments.size());
    description.pInputAttachments = input_attachments.data();
}

//-----------------------------------------------------------------------------
void subpass::set_resolve_attachment(index attachment, VkImageLayout layout) {
    VkAttachmentReference reference;
    reference.attachment = attachment;
    reference.layout = layout;

    set_resolve_attachment(reference);
}

//-----------------------------------------------------------------------------
void subpass::set_resolve_attachment(VkAttachmentReference attachment) {
    VkAttachmentReferences attachments;
    attachments.push_back(attachment);

    set_resolve_attachments(attachments);
}

//-----------------------------------------------------------------------------
void subpass::set_resolve_attachments(VkAttachmentReferences const& attachments) {
    resolve_attachments = attachments;

    description.pResolveAttachments = resolve_attachments.data();
}

//-----------------------------------------------------------------------------
void subpass::add_preserve_attachment(ui32 attachment) {
    preserve_attachments.push_back(attachment);

    set_preserve_attachments(preserve_attachments);
}

//-----------------------------------------------------------------------------
void subpass::set_preserve_attachments(index_list const& attachments) {
    preserve_attachments = attachments;

    description.preserveAttachmentCount = to_ui32(preserve_attachments.size());
    description.pPreserveAttachments = preserve_attachments.data();
}

//-----------------------------------------------------------------------------
void subpass::process(VkCommandBuffer cmd_buf, uv2 size) {
    for (auto& pipeline : pipelines) {
        if (!pipeline->activated())
            continue;

        if (!pipeline->on_process)
            continue;

        if (pipeline->auto_bind())
            pipeline->bind(cmd_buf);

        if (pipeline->auto_sizing())
            pipeline->set_viewport_and_scissor(cmd_buf, size);

        if (pipeline->auto_line_width())
            pipeline->set_line_width(cmd_buf);

        pipeline->on_process(cmd_buf);
    }
}

//-----------------------------------------------------------------------------
subpass::ptr make_subpass(VkPipelineBindPoint pipeline_bind_point) {
    auto result = std::make_shared<subpass>();

    result->set(pipeline_bind_point);
    return result;
}

//-----------------------------------------------------------------------------
subpass_dependency::subpass_dependency() {
    dependency.srcSubpass = 0;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = 0;
    dependency.dependencyFlags = 0;
}

//-----------------------------------------------------------------------------
subpass_dependency::ptr make_subpass_dependency(ui32 src_subpass, ui32 dst_subpass, VkDependencyFlags dependency_flags) {
    auto dependency = std::make_shared<subpass_dependency>();

    dependency->set_subpass(src_subpass, dst_subpass);
    dependency->set_dependency_flags(dependency_flags);

    return dependency;
}

} // namespace lava
