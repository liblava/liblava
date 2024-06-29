/**
 * @file         liblava/block/subpass.cpp
 * @brief        Subpass
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/block/subpass.hpp"
#include "liblava/core/misc.hpp"

namespace lava {

//-----------------------------------------------------------------------------
subpass::subpass() {
    m_description.flags = 0;
    m_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    m_description.inputAttachmentCount = 0;
    m_description.pInputAttachments = nullptr;
    m_description.colorAttachmentCount = 0;
    m_description.pColorAttachments = nullptr;
    m_description.pResolveAttachments = nullptr;
    m_description.pDepthStencilAttachment = nullptr;
    m_description.preserveAttachmentCount = 0;
    m_description.pPreserveAttachments = nullptr;
}

//-----------------------------------------------------------------------------
void subpass::destroy() {
    clear_pipelines();
}

//-----------------------------------------------------------------------------
void subpass::clear_pipelines() {
    for (auto& pipeline : m_pipelines)
        pipeline->destroy();

    m_pipelines.clear();
}

//-----------------------------------------------------------------------------
void subpass::remove(render_pipeline::s_ptr pipeline) {
    lava::remove(m_pipelines, std::move(pipeline));
}

//-----------------------------------------------------------------------------
void subpass::set_color_attachment(index attachment,
                                   VkImageLayout layout) {
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
    m_color_attachments = attachments;

    m_description.colorAttachmentCount = to_ui32(m_color_attachments.size());
    m_description.pColorAttachments = m_color_attachments.data();
}

//-----------------------------------------------------------------------------
void subpass::set_depth_stencil_attachment(index attachment,
                                           VkImageLayout layout) {
    VkAttachmentReference reference;
    reference.attachment = attachment;
    reference.layout = layout;

    set_depth_stencil_attachment(reference);
}

//-----------------------------------------------------------------------------
void subpass::set_depth_stencil_attachment(VkAttachmentReference attachment) {
    m_depth_stencil_attachment = attachment;

    m_description.pDepthStencilAttachment = &m_depth_stencil_attachment;
}

//-----------------------------------------------------------------------------
void subpass::set_input_attachment(index attachment,
                                   VkImageLayout layout) {
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
    m_input_attachments = attachments;

    m_description.inputAttachmentCount = to_ui32(m_input_attachments.size());
    m_description.pInputAttachments = m_input_attachments.data();
}

//-----------------------------------------------------------------------------
void subpass::set_resolve_attachment(index attachment,
                                     VkImageLayout layout) {
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
    m_resolve_attachments = attachments;

    m_description.pResolveAttachments = m_resolve_attachments.data();
}

//-----------------------------------------------------------------------------
void subpass::add_preserve_attachment(ui32 attachment) {
    m_preserve_attachments.push_back(attachment);

    set_preserve_attachments(m_preserve_attachments);
}

//-----------------------------------------------------------------------------
void subpass::set_preserve_attachments(index_list const& attachments) {
    m_preserve_attachments = attachments;

    m_description.preserveAttachmentCount = to_ui32(m_preserve_attachments.size());
    m_description.pPreserveAttachments = m_preserve_attachments.data();
}

//-----------------------------------------------------------------------------
void subpass::process(VkCommandBuffer cmd_buf,
                      uv2 size) {
    for (auto& pipeline : m_pipelines) {
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
subpass_dependency::subpass_dependency() {
    m_dependency.srcSubpass = 0;
    m_dependency.dstSubpass = 0;
    m_dependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    m_dependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    m_dependency.srcAccessMask = 0;
    m_dependency.dstAccessMask = 0;
    m_dependency.dependencyFlags = 0;
}

} // namespace lava
