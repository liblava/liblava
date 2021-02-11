// file      : liblava/block/attachment.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/base.hpp>

namespace lava {

    struct attachment : id_obj {
        using ptr = std::shared_ptr<attachment>;
        using list = std::vector<ptr>;

        explicit attachment(VkFormat format = VK_FORMAT_UNDEFINED, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT) {
            description.flags = 0;
            description.format = format;
            description.samples = samples;
            description.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            description.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        }

        VkAttachmentDescription const& get_description() const {
            return description;
        }

        void set_format(VkFormat format) {
            description.format = format;
        }
        void set_samples(VkSampleCountFlagBits samples) {
            description.samples = samples;
        }

        void set_op(VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op) {
            set_load_op(load_op);
            set_store_op(store_op);
        }

        void set_load_op(VkAttachmentLoadOp load_op) {
            description.loadOp = load_op;
        }
        void set_store_op(VkAttachmentStoreOp store_op) {
            description.storeOp = store_op;
        }

        void set_stencil_op(VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op) {
            set_stencil_load_op(load_op);
            set_stencil_store_op(store_op);
        }

        void set_stencil_load_op(VkAttachmentLoadOp load_op) {
            description.stencilLoadOp = load_op;
        }
        void set_stencil_store_op(VkAttachmentStoreOp store_op) {
            description.stencilStoreOp = store_op;
        }

        void set_layouts(VkImageLayout initial, VkImageLayout final) {
            set_initial_layout(initial);
            set_final_layout(final);
        }

        void set_initial_layout(VkImageLayout layout) {
            description.initialLayout = layout;
        }
        void set_final_layout(VkImageLayout layout) {
            description.finalLayout = layout;
        }

    private:
        VkAttachmentDescription description;
    };

    inline attachment::ptr make_attachment(VkFormat format = VK_FORMAT_UNDEFINED,
                                           VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT) {
        return std::make_shared<attachment>(format, samples);
    }

} // namespace lava
