// file      : liblava/block/subpass.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/block/pipeline.hpp>

namespace lava {

struct subpass : id_obj {

    using ptr = std::shared_ptr<subpass>;
    using list = std::vector<ptr>;

    static ptr make(VkPipelineBindPoint pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);
    void destroy();

    explicit subpass();

    void add(graphics_pipeline::ptr const& pipeline) { pipelines.push_back(pipeline); }

    void addFirst(graphics_pipeline::ptr const& pipeline) { pipelines.insert(pipelines.begin(), pipeline); }

    void remove(graphics_pipeline::ptr pipeline);

    void clear_pipelines();

    void process(VkCommandBuffer cmd_buf, uv2 size);;

    VkSubpassDescription get_description() const { return description; }

    void set(VkPipelineBindPoint pipeline_bind_point) { description.pipelineBindPoint = pipeline_bind_point; }

    void set_color_attachment(index attachment, VkImageLayout layout);
    void set_color_attachment(VkAttachmentReference attachment);
    void set_color_attachments(VkAttachmentReferences const& attachments);

    void set_depth_stencil_attachment(index attachment, VkImageLayout layout);
    void set_depth_stencil_attachment(VkAttachmentReference attachment);

    void set_input_attachment(index attachment, VkImageLayout layout);
    void set_input_attachment(VkAttachmentReference attachment);
    void set_input_attachments(VkAttachmentReferences const& attachments);

    void set_resolve_attachment(index attachment, VkImageLayout layout);
    void set_resolve_attachment(VkAttachmentReference attachment);
    void set_resolve_attachments(VkAttachmentReferences const& attachments);

    void add_preserve_attachment(index attachment);
    void set_preserve_attachments(index_list const& attachments);

    void set_active(bool value = true) { active = value; }
    bool is_active() const { return active; }

private:
    VkSubpassDescription description;
    bool active = true;

    VkAttachmentReferences color_attachments;
    VkAttachmentReference depth_stencil_attachment;
    VkAttachmentReferences input_attachments;
    VkAttachmentReferences resolve_attachments;
    index_list preserve_attachments;

    graphics_pipeline::list pipelines;
};

struct subpass_dependency : id_obj {

    using ptr = std::shared_ptr<subpass_dependency>;
    using list = std::vector<ptr>;

    static ptr make(ui32 src_subpass, ui32 dst_subpass,
                    VkDependencyFlags dependency_flags = VK_DEPENDENCY_BY_REGION_BIT);

    explicit subpass_dependency();

    VkSubpassDependency get_dependency() const { return dependency; }

    void set_subpass(ui32 src, ui32 dst) {
        
        set_src_subpass(src);
        set_dst_subpass(dst);
    }
    
    void set_src_subpass(ui32 src) { dependency.srcSubpass = src; }
    void set_dst_subpass(ui32 dst) { dependency.dstSubpass = dst; }

    void set_stage_mask(VkPipelineStageFlags src, VkPipelineStageFlags dst) {
        
        set_src_stage_mask(src);
        set_dst_stage_mask(dst);
    }

    void set_src_stage_mask(VkPipelineStageFlags mask) { dependency.srcStageMask = mask; }
    void set_dst_stage_mask(VkPipelineStageFlags mask) { dependency.dstStageMask = mask; }

    void set_access_mask(VkAccessFlags src, VkAccessFlags dst) {

        set_src_access_mask(src);
        set_dst_access_mask(dst);
    }

    void set_src_access_mask(VkAccessFlags mask) { dependency.srcAccessMask = mask; }
    void set_dst_access_mask(VkAccessFlags mask) { dependency.dstAccessMask = mask; }

    void set_dependency_flags(VkDependencyFlags flags) { dependency.dependencyFlags = flags; }

private:
    VkSubpassDependency dependency;
};

} // lava
