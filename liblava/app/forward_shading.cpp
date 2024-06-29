/**
 * @file         liblava/app/forward_shading.cpp
 * @brief        Forward shading
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/app/forward_shading.hpp"
#include "liblava/resource/format.hpp"

namespace lava {

//-----------------------------------------------------------------------------
bool forward_shading::create(render_target::s_ptr t) {
    m_target = t;

    auto depth_format = find_supported_depth_format(m_target->get_device()->get_vk_physical_device());
    if (!depth_format.has_value())
        return false;

    m_pass = render_pass::make(m_target->get_device());
    {
        auto color_attachment = attachment::make(m_target->get_format());
        color_attachment->set_op(VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
        color_attachment->set_stencil_op(VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                         VK_ATTACHMENT_STORE_OP_DONT_CARE);
        color_attachment->set_layouts(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        m_pass->add(color_attachment);

        auto depth_attachment = attachment::make(*depth_format);
        depth_attachment->set_op(VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);
        depth_attachment->set_stencil_op(VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                         VK_ATTACHMENT_STORE_OP_DONT_CARE);
        depth_attachment->set_layouts(VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        m_pass->add(depth_attachment);

        auto subpass = subpass::make(VK_PIPELINE_BIND_POINT_GRAPHICS);
        subpass->set_color_attachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        subpass->set_depth_stencil_attachment(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        m_pass->add(subpass);

        auto first_subpass_dependency = subpass_dependency::make(VK_SUBPASS_EXTERNAL, 0);
        first_subpass_dependency->set_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                                                     | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                                                     | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT);
        first_subpass_dependency->set_access_mask(VK_ACCESS_MEMORY_READ_BIT,
                                                  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
                                                      | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                                                      | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
                                                      | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
        m_pass->add(first_subpass_dependency);

        auto second_subpass_dependency = subpass_dependency::make(0, VK_SUBPASS_EXTERNAL);
        second_subpass_dependency->set_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                                                      | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                                  VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
        second_subpass_dependency->set_access_mask(VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
                                                       | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                                                       | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
                                                       | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                                   VK_ACCESS_MEMORY_READ_BIT
                                                       | VK_ACCESS_MEMORY_WRITE_BIT);
        m_pass->add(second_subpass_dependency);
    }

    m_depth_stencil = image::make(*depth_format);
    if (!m_depth_stencil)
        return false;

    m_depth_stencil->set_usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
                               | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    m_depth_stencil->set_layout(VK_IMAGE_LAYOUT_UNDEFINED);
    m_depth_stencil->set_aspect_mask(VK_IMAGE_ASPECT_DEPTH_BIT
                                     | VK_IMAGE_ASPECT_STENCIL_BIT);
    m_depth_stencil->set_component();

    m_target->on_create_attachments = [&]() -> VkAttachments {
        VkAttachments result;

        if (!m_depth_stencil->create(m_target->get_device(), m_target->get_size()))
            return {};

        for (auto& backbuffer : m_target->get_backbuffers()) {
            VkImageViews attachments;

            attachments.push_back(backbuffer->get_view());
            attachments.push_back(m_depth_stencil->get_view());

            result.push_back(attachments);
        }

        return result;
    };

    m_target->on_destroy_attachments = [&]() {
        m_depth_stencil->destroy();
    };

    if (!m_pass->create(m_target->on_create_attachments(), {{}, m_target->get_size()}))
        return false;

    m_target->add_callback(&m_pass->get_target_callback());

    m_pass->set_clear_color();

    return true;
}

//-----------------------------------------------------------------------------
void forward_shading::destroy() {
    if (!m_target)
        return;

    m_target->remove_callback(&m_pass->get_target_callback());

    m_pass->destroy();
    m_pass = nullptr;

    m_depth_stencil->destroy();
    m_depth_stencil = nullptr;

    m_target = nullptr;
}

} // namespace lava
