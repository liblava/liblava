/**
 * @file         liblava/block/attachment.hpp
 * @brief        Attachment description
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/base/base.hpp"
#include <memory>

namespace lava {

/**
 * @brief Attachment description
 */
struct attachment {
    /// Shared pointer to attachment
    using s_ptr = std::shared_ptr<attachment>;

    /// List of attachments
    using s_list = std::vector<s_ptr>;

    /**
     * @brief Make a new attachment
     * @param format     Attachment format
     * @param samples    Sample count flag bits
     * @return s_ptr     Shared pointer to attachment
     */
    static s_ptr make(VkFormat format = VK_FORMAT_UNDEFINED,
                      VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT) {
        return std::make_shared<attachment>(format, samples);
    }

    /**
     * @brief Construct a new attachment
     * @param format     Attachment format
     * @param samples    Sample count flag bits
     */
    explicit attachment(VkFormat format = VK_FORMAT_UNDEFINED,
                        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT) {
        m_description.flags = 0;
        m_description.format = format;
        m_description.samples = samples;
        m_description.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        m_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        m_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        m_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        m_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        m_description.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

    /**
     * @brief Get the description
     * @return VkAttachmentDescription const&    Attachment description
     */
    VkAttachmentDescription const& get_description() const {
        return m_description;
    }

    /**
     * @brief Set the format
     * @param format    Attachment format
     */
    void set_format(VkFormat format) {
        m_description.format = format;
    }

    /**
     * @brief Set the samples
     * @param samples    Attachment sample count flag bits
     */
    void set_samples(VkSampleCountFlagBits samples) {
        m_description.samples = samples;
    }

    /**
     * @brief Set the op
     * @param load_op     Attachment load op
     * @param store_op    Attachment store op
     */
    void set_op(VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op) {
        set_load_op(load_op);
        set_store_op(store_op);
    }

    /**
     * @brief Set the load op
     * @param load_op    Attachment load op
     */
    void set_load_op(VkAttachmentLoadOp load_op) {
        m_description.loadOp = load_op;
    }

    /**
     * @brief Set the store op
     * @param store_op    Attachment store op
     */
    void set_store_op(VkAttachmentStoreOp store_op) {
        m_description.storeOp = store_op;
    }

    /**
     * @brief Set the stencil op
     * @param load_op     Attachment load op
     * @param store_op    Attachment store op
     */
    void set_stencil_op(VkAttachmentLoadOp load_op,
                        VkAttachmentStoreOp store_op) {
        set_stencil_load_op(load_op);
        set_stencil_store_op(store_op);
    }

    /**
     * @brief Set the stencil load op
     * @param load_op    Attachment load op
     */
    void set_stencil_load_op(VkAttachmentLoadOp load_op) {
        m_description.stencilLoadOp = load_op;
    }

    /**
     * @brief Set the stencil store op
     * @param store_op    Attachment store op
     */
    void set_stencil_store_op(VkAttachmentStoreOp store_op) {
        m_description.stencilStoreOp = store_op;
    }

    /**
     * @brief Set the layouts
     * @param initial    Initial image layout
     * @param final      Final image layout
     */
    void set_layouts(VkImageLayout initial,
                     VkImageLayout final) {
        set_initial_layout(initial);
        set_final_layout(final);
    }

    /**
     * @brief Set the initial layout
     * @param layout    Image layout
     */
    void set_initial_layout(VkImageLayout layout) {
        m_description.initialLayout = layout;
    }

    /**
     * @brief Set the final layout
     * @param layout    Image layout
     */
    void set_final_layout(VkImageLayout layout) {
        m_description.finalLayout = layout;
    }

private:
    /// Attachment description
    VkAttachmentDescription m_description;
};

} // namespace lava
