/**
 * @file         liblava/block/render_pass.hpp
 * @brief        Render pass
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/base/device.hpp"
#include "liblava/block/attachment.hpp"
#include "liblava/block/subpass.hpp"

namespace lava {

/**
 * @brief Render pass
 */
struct render_pass : entity {
    /// Shared pointer to render pass
    using s_ptr = std::shared_ptr<render_pass>;

    /// List of render passes
    using s_list = std::vector<s_ptr>;

    /**
     * @brief Make a new render pass
     * @param device    Vulkan device
     * @return s_ptr    Shared pointer to render pass
     */
    static s_ptr make(device::ptr device) {
        return std::make_shared<render_pass>(device);
    }

    /**
     * @brief Construct a new render pass
     * @param device    Vulkan device
     */
    explicit render_pass(device::ptr device);

    /**
     * @brief Create a new render pass
     * @param target_attachments    List of target attachments
     * @param area                  Rectangle area
     * @return Create was successful or failed
     */
    bool create(VkAttachmentsRef target_attachments,
                rect::ref area);

    /**
     * @brief Destroy the render pass
     */
    void destroy();

    /**
     * @brief Process the render pass
     * @param cmd_buf    Command buffer
     * @param frame      Frame index
     */
    void process(VkCommandBuffer cmd_buf,
                 index frame);

    /**
     * @brief Get the device
     * @return device::ptr    Vulkan device
     */
    device::ptr get_device() {
        return m_device;
    }

    /**
     * @brief Get the render pass
     * @return VkRenderPass    Vulkan render pass
     */
    VkRenderPass get() const {
        return m_vk_render_pass;
    }

    /**
     * @brief Get the subpass count
     * @return ui32    Number of subpasses
     */
    ui32 get_subpass_count() const {
        return to_ui32(m_subpasses.size());
    }

    /**
     * @brief Check if subpass exists
     * @param index     Index to check
     * @return Subpass exists or not
     */
    bool exists_subpass(index index = 0) const {
        return index < m_subpasses.size();
    }

    /**
     * @brief Get the subpass
     * @param index        Index of subpass
     * @return subpass*    Subpass
     */
    subpass* get_subpass(index index = 0) {
        return m_subpasses.at(index).get();
    }

    /**
     * @brief Get the subpasses
     * @return subpass::s_list const&    List of subpasses
     */
    subpass::s_list const& get_subpasses() const {
        return m_subpasses;
    }

    /**
     * @brief Add an attachment
     * @param attachment    Attachment
     */
    void add(attachment::s_ptr const& attachment) {
        m_attachments.push_back(attachment);
    }

    /**
     * @brief Add a subpass dependency
     * @param dependency    Subpass dependency
     */
    void add(subpass_dependency::s_ptr const& dependency) {
        m_dependencies.push_back(dependency);
    }

    /**
     * @brief Add a subpass
     * @param subpass    Subpass
     */
    void add(subpass::s_ptr const& subpass) {
        m_subpasses.push_back(subpass);
    }

    /**
     * @brief Set the clear values
     * @param values    List of clear values
     */
    void set_clear_values(VkClearValues const& values) {
        m_clear_values = values;
    }

    /**
     * @brief Get the clear values
     * @return VkClearValues const&    List of clear values
     */
    VkClearValues const& get_clear_values() const {
        return m_clear_values;
    }

    /**
     * @brief Set the clear color
     * @param value    Clear color
     */
    void set_clear_color(v3 value = {});

    /**
     * @brief Get the clear color
     * @return v3    Clear color
     */
    v3 get_clear_color() const;

    /**
     * @brief Add a render pipeline to the back of subpass
     * @param pipeline    Render pipeline
     * @param subpass     Subpass
     */
    void add(render_pipeline::s_ptr pipeline,
             index subpass = 0) {
        m_subpasses.at(subpass)->add(pipeline);
    }

    /**
     * @brief Add a render pipeline to the front of subpass
     * @param pipeline    Render pipeline
     * @param subpass     Subpass
     */
    void add_front(render_pipeline::s_ptr pipeline,
                   index subpass = 0) {
        m_subpasses.at(subpass)->add_front(pipeline);
    }

    /**
     * @brief Remove a render pipeline from the subpass
     * @param pipeline    Render pipeline
     * @param subpass     Subpass
     */
    void remove(render_pipeline::s_ptr pipeline,
                index subpass = 0) {
        m_subpasses.at(subpass)->remove(pipeline);
    }

    /**
     * @brief Get the target callback
     * @return target_callback const&    Target callback
     */
    target_callback const& get_target_callback() const {
        return m_callback;
    }

private:
    /// Vulkan device
    device::ptr m_device = nullptr;

    /// Vulkan render pass
    VkRenderPass m_vk_render_pass = VK_NULL_HANDLE;

    /// List of frame buffers
    VkFramebuffers m_framebuffers = {};

    /// List of attachments
    attachment::s_list m_attachments;

    /// List of subpass dependencies
    subpass_dependency::s_list m_dependencies;

    /// List of subpasses
    subpass::s_list m_subpasses;

    /// List of clear values
    VkClearValues m_clear_values = {};

    /// Rectangle area
    rect m_area;

    /// Target callback
    target_callback m_callback;

    /**
     * @brief Begin the render pass
     * @param cmd_buf    Command buffer
     * @param frame      Frame index
     */
    void begin(VkCommandBuffer cmd_buf,
               index frame);

    /**
     * @brief End the render pass
     * @param cmd_buf    Command buffer
     */
    void end(VkCommandBuffer cmd_buf);

    /**
     * @brief Called on target created
     * @param target_attachments    List of target attachments
     * @param area                  Rectangle area
     * @return Create was successful or failed
     */
    bool on_target_created(VkAttachmentsRef target_attachments,
                           rect::ref area);

    /**
     * @brief Called on target destroyed
     */
    void on_target_destroyed();
};

} // namespace lava
