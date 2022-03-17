/**
 * @file         liblava/block/render_pass.hpp
 * @brief        Render pass
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/device.hpp>
#include <liblava/block/attachment.hpp>
#include <liblava/block/subpass.hpp>

namespace lava {

/**
 * @brief Render pass
 */
struct render_pass : entity {
    /// Shared pointer to render pass
    using ptr = std::shared_ptr<render_pass>;

    /// List of render passes
    using list = std::vector<ptr>;

    /**
     * @brief Construct a new render pass
     *
     * @param device    Vulkan device
     */
    explicit render_pass(device_ptr device);

    /**
     * @brief Create a new render pass
     *
     * @param target_attachments    List of target attachments
     * @param area                  Rectangle area
     *
     * @return true                 Create was successful
     * @return false                Create failed
     */
    bool create(VkAttachmentsRef target_attachments, rect area);

    /**
     * @brief Destroy the render pass
     */
    void destroy();

    /**
     * @brief Process the render pass
     *
     * @param cmd_buf    Command buffer
     * @param frame      Frame index
     */
    void process(VkCommandBuffer cmd_buf, index frame);

    /**
     * @brief Get the device
     *
     * @return device_ptr    Vulkan device
     */
    device_ptr get_device() {
        return device;
    }

    /**
     * @brief Get the render pass
     *
     * @return VkRenderPass    Vulkan render pass
     */
    VkRenderPass get() const {
        return vk_render_pass;
    }

    /**
     * @brief Get the subpass count
     *
     * @return ui32    Number of subpasses
     */
    ui32 get_subpass_count() const {
        return to_ui32(subpasses.size());
    }

    /**
     * @brief Check if subpass exists
     *
     * @param index     Index to check
     *
     * @return true     Subpass exists
     * @return false    Subpass does not exist
     */
    bool exists_subpass(index index = 0) const {
        return index < subpasses.size();
    }

    /**
     * @brief Get the subpass
     *
     * @param index        Index of subpass
     *
     * @return subpass*    Subpass
     */
    subpass* get_subpass(index index = 0) {
        return subpasses.at(index).get();
    }

    /**
     * @brief Get the subpasses
     *
     * @return subpass::list const&    List of subpasses
     */
    subpass::list const& get_subpasses() const {
        return subpasses;
    }

    /**
     * @brief Add an attachment
     *
     * @param attachment    Attachment
     */
    void add(attachment::ptr const& attachment) {
        attachments.push_back(attachment);
    }

    /**
     * @brief Add a subpass dependency
     *
     * @param dependency    Subpass dependency
     */
    void add(subpass_dependency::ptr const& dependency) {
        dependencies.push_back(dependency);
    }

    /**
     * @brief Add a subpass
     *
     * @param subpass    Subpass
     */
    void add(subpass::ptr const& subpass) {
        subpasses.push_back(subpass);
    }

    /**
     * @brief Set the clear values
     *
     * @param values    List of clear values
     */
    void set_clear_values(VkClearValues const& values) {
        clear_values = values;
    }

    /**
     * @brief Get the clear values
     *
     * @return VkClearValues const&    List of clear values
     */
    VkClearValues const& get_clear_values() const {
        return clear_values;
    }

    /**
     * @brief Set the clear color
     *
     * @param value    Clear color
     */
    void set_clear_color(v3 value = v3(0.086f, 0.086f, 0.094f));

    /**
     * @brief Get the clear color
     *
     * @return v3    Clear color
     */
    v3 get_clear_color() const;

    /**
     * @brief Add a graphics pipeline to the back of subpass
     *
     * @param pipeline    Graphics pipeline
     * @param subpass     Subpass
     */
    void add(graphics_pipeline::ptr pipeline, index subpass = 0) {
        subpasses.at(subpass)->add(pipeline);
    }

    /**
     * @brief Add a graphics pipeline to the front of subpass
     *
     * @param pipeline    Graphics pipeline
     * @param subpass     Subpass
     */
    void add_front(graphics_pipeline::ptr pipeline, index subpass = 0) {
        subpasses.at(subpass)->add_front(pipeline);
    }

    /**
     * @brief Remove a graphics pipeline from the subpass
     *
     * @param pipeline    Graphics pipeline
     * @param subpass     Subpass
     */
    void remove(graphics_pipeline::ptr pipeline, index subpass = 0) {
        subpasses.at(subpass)->remove(pipeline);
    }

    /**
     * @brief Get the target callback
     *
     * @return target_callback const&    Target callback
     */
    target_callback const& get_target_callback() const {
        return callback;
    }

private:
    /// Vulkan device
    device_ptr device = nullptr;

    /// Vulkan render pass
    VkRenderPass vk_render_pass = VK_NULL_HANDLE;

    /// List of frame buffers
    VkFramebuffers framebuffers = {};

    /// List of attachments
    attachment::list attachments;

    /// List of subpass dependencies
    subpass_dependency::list dependencies;

    /// List of subpasses
    subpass::list subpasses;

    /// List of clear values
    VkClearValues clear_values = {};

    /// Rectangle area
    rect area;

    /// Target callback
    target_callback callback;

    /**
     * @brief Begin the render pass
     *
     * @param cmd_buf    Command buffer
     * @param frame      Frame index
     */
    void begin(VkCommandBuffer cmd_buf, index frame);

    /**
     * @brief End the render pass
     *
     * @param cmd_buf    Command buffer
     */
    void end(VkCommandBuffer cmd_buf);

    /**
     * @brief Called on target created
     *
     * @param target_attachments    List of target attachments
     * @param area                  Rectangle area
     *
     * @return true                 Create was successful
     * @return false                Create failed
     */
    bool on_target_created(VkAttachmentsRef target_attachments, rect area);

    /**
     * @brief Called on target destroyed
     */
    void on_target_destroyed();
};

/**
 * @brief Make a new render pass
 *
 * @param device               Vulkan device
 *
 * @return render_pass::ptr    Shared pointer to render pass
 */
inline render_pass::ptr make_render_pass(device_ptr device) {
    return std::make_shared<render_pass>(device);
}

} // namespace lava
