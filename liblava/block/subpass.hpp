/**
 * @file         liblava/block/subpass.hpp
 * @brief        Subpass
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/block/render_pipeline.hpp>

namespace lava {

/**
 * @brief Subpass
 */
struct subpass : entity {
    /// Shared pointer to subpass
    using ptr = std::shared_ptr<subpass>;

    /// List of subpasses
    using list = std::vector<ptr>;

    /**
     * @brief Construct a new subpass
     */
    explicit subpass();

    /**
     * @brief Destroy the subpass
     */
    void destroy();

    /**
     * @brief Add a render pipeline to the back of the subpass
     * @param pipeline    Render pipeline
     */
    void add(render_pipeline::ptr const& pipeline) {
        pipelines.push_back(pipeline);
    }

    /**
     * @brief Add a render pipeline to the fronst of the subpass
     * @param pipeline    Render pipeline
     */
    void add_front(render_pipeline::ptr const& pipeline) {
        pipelines.insert(pipelines.begin(), pipeline);
    }

    /**
     * @brief Remove the render pipeline
     * @param pipeline    Render pipeline
     */
    void remove(render_pipeline::ptr pipeline);

    /**
     * @brief Clear all pipelines
     */
    void clear_pipelines();

    /**
     * @brief Process the subpass
     * @param cmd_buf    Command buffer
     * @param size       Size of render pass
     */
    void process(VkCommandBuffer cmd_buf,
                 uv2 size);

    /**
     * @brief Get the description
     * @return VkSubpassDescription const&    Subpass description
     */
    VkSubpassDescription const& get_description() const {
        return description;
    }

    /**
     * @brief Set pipeline bind point
     * @param pipeline_bind_point    Pipeline bind point
     */
    void set(VkPipelineBindPoint pipeline_bind_point) {
        description.pipelineBindPoint = pipeline_bind_point;
    }

    /**
     * @brief Set the color attachment
     * @param attachment    Index of attachment
     * @param layout        Image layout
     */
    void set_color_attachment(index attachment,
                              VkImageLayout layout);

    /**
     * @brief Set the color attachment
     * @param attachment    Attachment reference
     */
    void set_color_attachment(VkAttachmentReference attachment);

    /**
     * @brief Set the color attachments
     * @param attachments    List of attachment references
     */
    void set_color_attachments(VkAttachmentReferences const& attachments);

    /**
     * @brief Set the depth stencil attachment
     * @param attachment    Index of attachment
     * @param layout        Image layout
     */
    void set_depth_stencil_attachment(index attachment,
                                      VkImageLayout layout);

    /**
     * @brief Set the depth stencil attachment
     * @param attachment    Attachment reference
     */
    void set_depth_stencil_attachment(VkAttachmentReference attachment);

    /**
     * @brief Set the input attachment
     * @param attachment    Index of attachment
     * @param layout        Image layout
     */
    void set_input_attachment(index attachment,
                              VkImageLayout layout);

    /**
     * @brief Set the input attachment
     * @param attachment    Attachment reference
     */
    void set_input_attachment(VkAttachmentReference attachment);

    /**
     * @brief Set the input attachments
     * @param attachments    List of attachment references
     */
    void set_input_attachments(VkAttachmentReferences const& attachments);

    /**
     * @brief Set the resolve attachment
     * @param attachment    Index of attachment
     * @param layout        Image layout
     */
    void set_resolve_attachment(index attachment, VkImageLayout layout);

    /**
     * @brief Set the resolve attachment
     * @param attachment    Attachment reference
     */
    void set_resolve_attachment(VkAttachmentReference attachment);

    /**
     * @brief Set the resolve attachments
     * @param attachments    List of attachment references
     */
    void set_resolve_attachments(VkAttachmentReferences const& attachments);

    /**
     * @brief Add preserve attachment
     * @param attachment    Index of attachment
     */
    void add_preserve_attachment(index attachment);

    /**
     * @brief Set the preserve attachments
     * @param attachments    List of indices
     */
    void set_preserve_attachments(index_list const& attachments);

    /**
     * @brief Activate or deactivate the subpass
     * @param value    Enable state
     */
    void set_active(bool value = true) {
        active = value;
    }

    /**
     * @brief Check if subpass is active
     * @return true     Subpass is active
     * @return false    Subpass is inactive
     */
    bool activated() const {
        return active;
    }

private:
    /// Vulkan subpass description
    VkSubpassDescription description;

    //// Active state
    bool active = true;

    /// List of color attachment references
    VkAttachmentReferences color_attachments;

    /// Depth stencil attachment reference
    VkAttachmentReference depth_stencil_attachment{};

    /// List of input attachment references
    VkAttachmentReferences input_attachments;

    /// List of resolve attachment references
    VkAttachmentReferences resolve_attachments;

    /// List of preserve attachment indicies
    index_list preserve_attachments;

    /// List of render pipelines
    render_pipeline::list pipelines;
};

/**
 * @brief Make a new subpass
 * @param pipeline_bind_point    Pipeline bind point
 * @return subpass::ptr          Shared pointer to subpass
 */
subpass::ptr make_subpass(VkPipelineBindPoint pipeline_bind_point =
                              VK_PIPELINE_BIND_POINT_GRAPHICS);

/**
 * @brief Subpass dependency
 */
struct subpass_dependency {
    /// Shared pointer to subpass dependency
    using ptr = std::shared_ptr<subpass_dependency>;

    /// List of subpass dependencies
    using list = std::vector<ptr>;

    /**
     * @brief Construct a new subpass dependency
     */
    explicit subpass_dependency();

    /**
     * @brief Get the dependency
     * @return VkSubpassDependency const&    Vulkan subpass dependency
     */
    VkSubpassDependency const& get_dependency() const {
        return dependency;
    }

    /**
     * @brief Set the subpass
     * @param src    Source Subpass
     * @param dst    Destination Subpass
     */
    void set_subpass(ui32 src, ui32 dst) {
        set_src_subpass(src);
        set_dst_subpass(dst);
    }

    /**
     * @brief Set the source subpass
     * @param src    Source Subpass
     */
    void set_src_subpass(ui32 src) {
        dependency.srcSubpass = src;
    }

    /**
     * @brief Set the dst subpass
     * @param dst    Destination subpass
     */
    void set_dst_subpass(ui32 dst) {
        dependency.dstSubpass = dst;
    }

    /**
     * @brief Set the stage mask
     * @param src    Source pipeline stage flags
     * @param dst    Destination pipeline stage flags
     */
    void set_stage_mask(VkPipelineStageFlags src,
                        VkPipelineStageFlags dst) {
        set_src_stage_mask(src);
        set_dst_stage_mask(dst);
    }

    /**
     * @brief Set the source stage mask
     * @param mask    Pipeline stage flags
     */
    void set_src_stage_mask(VkPipelineStageFlags mask) {
        dependency.srcStageMask = mask;
    }

    /**
     * @brief Set the destination stage mask
     * @param mask    Pipeline stage flags
     */
    void set_dst_stage_mask(VkPipelineStageFlags mask) {
        dependency.dstStageMask = mask;
    }

    /**
     * @brief Set the access mask
     * @param src    Source access flags
     * @param dst    Destination access flags
     */
    void set_access_mask(VkAccessFlags src,
                         VkAccessFlags dst) {
        set_src_access_mask(src);
        set_dst_access_mask(dst);
    }

    /**
     * @brief Set the src access mask
     * @param mask    Access flags
     */
    void set_src_access_mask(VkAccessFlags mask) {
        dependency.srcAccessMask = mask;
    }

    /**
     * @brief Set the dst access mask
     * @param mask    Access flags
     */
    void set_dst_access_mask(VkAccessFlags mask) {
        dependency.dstAccessMask = mask;
    }

    /**
     * @brief Set the dependency flags
     * @param flags    Dependency flags
     */
    void set_dependency_flags(VkDependencyFlags flags) {
        dependency.dependencyFlags = flags;
    }

private:
    /// Vulkan subpass dependency
    VkSubpassDependency dependency;
};

/**
 * @brief Make a new subpass dependency
 * @param src_subpass                 Source subpass
 * @param dst_subpass                 Destination subpass
 * @param dependency_flags            Dependency flags
 * @return subpass_dependency::ptr    Shared pointer to subpass dependency
 */
subpass_dependency::ptr make_subpass_dependency(ui32 src_subpass,
                                                ui32 dst_subpass,
                                                VkDependencyFlags dependency_flags =
                                                    VK_DEPENDENCY_BY_REGION_BIT);

} // namespace lava
