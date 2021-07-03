/**
 * @file         liblava/app/forward_shading.cpp
 * @brief        Forward shading
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/app/forward_shading.hpp>
#include <liblava/resource/format.hpp>

namespace lava {

//-----------------------------------------------------------------------------
bool forward_shading::create(render_target::ptr t) {
    target = t;

    auto depth_format = get_supported_depth_format(target->get_device()->get_vk_physical_device());
    if (!depth_format.has_value())
        return false;

    pass = make_render_pass(target->get_device());
    {
        auto color_attachment = make_attachment(target->get_format());
        color_attachment->set_op(VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
        color_attachment->set_stencil_op(VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE);
        color_attachment->set_layouts(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        pass->add(color_attachment);

        auto depth_attachment = make_attachment(*depth_format);
        depth_attachment->set_op(VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);
        depth_attachment->set_stencil_op(VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE);
        depth_attachment->set_layouts(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        pass->add(depth_attachment);

        auto subpass = make_subpass(VK_PIPELINE_BIND_POINT_GRAPHICS);
        subpass->set_color_attachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        subpass->set_depth_stencil_attachment(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        pass->add(subpass);

        auto first_subpass_dependency = make_subpass_dependency(VK_SUBPASS_EXTERNAL, 0);
        first_subpass_dependency->set_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT);
        first_subpass_dependency->set_access_mask(VK_ACCESS_MEMORY_READ_BIT,
                                                  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
        pass->add(first_subpass_dependency);

        auto second_subpass_dependency = make_subpass_dependency(0, VK_SUBPASS_EXTERNAL);
        second_subpass_dependency->set_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                                  VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
        second_subpass_dependency->set_access_mask(VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                                   VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT);
        pass->add(second_subpass_dependency);
    }

    depth_stencil = make_image(*depth_format);
    if (!depth_stencil)
        return false;

    depth_stencil->set_usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    depth_stencil->set_layout(VK_IMAGE_LAYOUT_UNDEFINED);
    depth_stencil->set_aspect_mask(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
    depth_stencil->set_component();

    target->on_create_attachments = [&]() -> VkAttachments {
        VkAttachments result;

        if (!depth_stencil->create(target->get_device(), target->get_size()))
            return {};

        for (auto& backbuffer : target->get_backbuffers()) {
            VkImageViews attachments;

            attachments.push_back(backbuffer->get_view());
            attachments.push_back(depth_stencil->get_view());

            result.push_back(attachments);
        }

        return result;
    };

    target->on_destroy_attachments = [&]() {
        depth_stencil->destroy();
    };

    if (!pass->create(target->on_create_attachments(), { {}, target->get_size() }))
        return false;

    target->add_callback(&pass->get_target_callback());

    pass->set_clear_color();

    return true;
}

//-----------------------------------------------------------------------------
void forward_shading::destroy() {
    if (!target)
        return;

    target->remove_callback(&pass->get_target_callback());

    pass->destroy();
    pass = nullptr;

    depth_stencil->destroy();
    depth_stencil = nullptr;

    target = nullptr;
}

} // namespace lava
