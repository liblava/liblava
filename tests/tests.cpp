// file      : tests/tests.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <tests/driver.hpp>

using namespace lava;

LAVA_TEST(1, "frame init")
{
    frame frame(argh);

    return frame.ready() ? 0 : error::not_ready;
}

LAVA_TEST(2, "run loop")
{
    frame frame(argh);
    if (!frame.ready())
        return error::not_ready;

    auto count = 0;

    frame.add_run([&]() {

        sleep(1.0);
        count++;

        log()->debug("{} - running {} sec", count, frame.get_running_time());

        if (count == 3)
            frame.shut_down();

        return true;
    });

    return frame.run() ? 0 : error::aborted;
}

LAVA_TEST(3, "window input")
{
    frame frame(argh);
    if (!frame.ready())
        return error::not_ready;

    window window;
    if (!window.create())
        return error::create_failed;

    input input;
    window.assign(&input);

    input.key.listeners.add([&](key_event::ref event) {

        if (event.key == key::escape && event.action == action::press)
            frame.shut_down();
    });

    frame.add_run([&]() {

        input.handle_events();

        if (window.has_close_request())
            frame.shut_down();

        return true;
    });

    return frame.run() ? 0 : error::aborted;
}

LAVA_TEST(4, "clear color")
{
    frame frame(argh);
    if (!frame.ready())
        return error::not_ready;

    window window;
    if (!window.create())
        return error::create_failed;

    input input;
    window.assign(&input);

    input.key.listeners.add([&](key_event::ref event) {

        if (event.key == key::escape && event.action == action::press)
            frame.shut_down();
    });

    auto device = frame.create_device();
    if (!device)
        return error::create_failed;

    auto render_target = create_target(&window, device);
    if (!render_target)
        return error::create_failed;

    renderer simple_renderer;
    if (!simple_renderer.create(render_target->get_swapchain()))
        return error::create_failed;

    auto frame_count = render_target->get_frame_count();

    VkCommandPool cmd_pool;
    VkCommandBuffers cmd_bufs(frame_count);

    auto build_cmd_bufs = [&]() {

        if (!device->vkCreateCommandPool(device->get_graphics_queue().family, &cmd_pool))
            return false;

        if (!device->vkAllocateCommandBuffers(cmd_pool, frame_count, cmd_bufs.data()))
            return false;

        VkCommandBufferBeginInfo begin_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
        };

        VkClearColorValue clear_color = { random(0.f, 1.f), random(0.f, 1.f), random(0.f, 1.f), 0.f };

        VkImageSubresourceRange image_range
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1,
        };

        for (auto i = 0u; i < frame_count; ++i) {

            auto cmd_buf = cmd_bufs[i];
            auto frame_image = render_target->get_backbuffer_image(i);

            if (failed(device->call().vkBeginCommandBuffer(cmd_buf, &begin_info)))
                return false;

            insert_image_memory_barrier(device, cmd_buf, frame_image,
                                        VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                                        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, image_range);

            device->call().vkCmdClearColorImage(cmd_buf, frame_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_color, 1, &image_range);

            insert_image_memory_barrier(device, cmd_buf, frame_image,
                                        VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, image_range);

            if (failed(device->call().vkEndCommandBuffer(cmd_buf)))
                return false;
        }

        return true;
    };

    auto clean_cmd_bufs = [&]() {

        device->vkFreeCommandBuffers(cmd_pool, frame_count, cmd_bufs.data());
        device->vkDestroyCommandPool(cmd_pool);
    };

    if (!build_cmd_bufs())
        return error::create_failed;

    render_target->on_swapchain_start = build_cmd_bufs;
    render_target->on_swapchain_stop = clean_cmd_bufs;

    frame.add_run([&]() {

        input.handle_events();

        if (window.has_close_request())
            return frame.shut_down();

        if (window.has_resize_request())
            return window.handle_resize();

        if (window.iconified()) {

            frame.set_wait_for_events(true);
            return true;

        } else {

            if (frame.waiting_for_events())
                frame.set_wait_for_events(false);
        }

        auto frame_index = simple_renderer.begin_frame();
        if (!frame_index)
            return true;

        return simple_renderer.end_frame({ cmd_bufs[*frame_index] });
    });

    frame.add_run_end([&]() {

        clean_cmd_bufs();
        simple_renderer.destroy();
        render_target->destroy();
    });

    return frame.run() ? 0 : error::aborted;
}

LAVA_TEST(5, "color block")
{
    frame frame(argh);
    if (!frame.ready())
        return error::not_ready;

    window window;
    if (!window.create())
        return error::create_failed;

    input input;
    window.assign(&input);

    input.key.listeners.add([&](key_event::ref event) {

        if (event.key == key::escape && event.action == action::press)
            frame.shut_down();
    });

    auto device = frame.create_device();
    if (!device)
        return error::create_failed;

    auto render_target = create_target(&window, device);
    if (!render_target)
        return error::create_failed;

    renderer simple_renderer;
    if (!simple_renderer.create(render_target->get_swapchain()))
        return error::create_failed;

    auto frame_count = render_target->get_frame_count();

    block block;

    if (!block.create(device, frame_count, device->get_graphics_queue().family))
        return error::create_failed;

    block.add_command([&](VkCommandBuffer cmd_buf) {

        VkClearColorValue clear_color = { random(0.f, 1.f), random(0.f, 1.f), random(0.f, 1.f), 0.f };

        VkImageSubresourceRange image_range
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1,
        };

        auto frame_image = render_target->get_backbuffer_image(block.get_current_frame());

        insert_image_memory_barrier(device, cmd_buf, frame_image,
                                    VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, image_range);

        device->call().vkCmdClearColorImage(cmd_buf, frame_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_color, 1, &image_range);

        insert_image_memory_barrier(device, cmd_buf, frame_image,
                                    VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, image_range);
    });

    frame.add_run([&]() {

        input.handle_events();

        if (window.has_close_request())
            return frame.shut_down();

        if (window.has_resize_request())
            return window.handle_resize();

        if (window.iconified()) {

            frame.set_wait_for_events(true);
            return true;

        } else {

            if (frame.waiting_for_events())
                frame.set_wait_for_events(false);
        }

        auto frame_index = simple_renderer.begin_frame();
        if (!frame_index)
            return true;

        if (!block.process(*frame_index))
            return false;

        return simple_renderer.end_frame(block.get_buffers());
    });

    frame.add_run_end([&]() {

        block.destroy();
        simple_renderer.destroy();
        render_target->destroy();
    });

    return frame.run() ? 0 : error::aborted;
}

LAVA_TEST(6, "gamepad listener")
{
    frame frame(argh);
    if (!frame.ready())
        return error::not_ready;

    gamepad_manager::add_listener([&](gamepad pad, bool active) {

        auto id = to_ui32(pad.get_id());

        if (active)
            log()->info("gamepad {} - active ({})", id, pad.get_name());
        else
            log()->info("gamepad {} - inactive", id);
    });

    for (auto& pad : gamepads())
        log()->info("gamepad {} - active ({})", to_ui32(pad.get_id()), pad.get_name());

    frame.add_run([&]() {

        sleep(1.0);

        return true;
    });

    return frame.run() ? 0 : error::aborted;
}

LAVA_TEST(7, "forward shading")
{
    frame frame(argh);
    if (!frame.ready())
        return error::not_ready;

    window window;
    if (!window.create())
        return error::create_failed;

    input input;
    window.assign(&input);

    input.key.listeners.add([&](key_event::ref event) {

        if (event.key == key::escape && event.action == action::press)
            frame.shut_down();
    });

    auto device = frame.create_device();
    if (!device)
        return error::create_failed;

    auto render_target = create_target(&window, device);
    if (!render_target)
        return error::create_failed;

    auto depth_format = VK_FORMAT_UNDEFINED;
    if (!get_supported_depth_format(device->get_vk_physical_device(), &depth_format))
        return error::create_failed;

    render_pass render_pass(device);
    {
        auto color_attachment = attachment::make(render_target->get_format());
        color_attachment->set_op(VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
        color_attachment->set_stencil_op(VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE);
        color_attachment->set_layouts(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        render_pass.add(color_attachment);

        auto depth_attachment = attachment::make(depth_format);
        depth_attachment->set_op(VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);
        depth_attachment->set_stencil_op(VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE);
        depth_attachment->set_layouts(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        render_pass.add(depth_attachment);

        auto subpass = subpass::make(VK_PIPELINE_BIND_POINT_GRAPHICS);
        subpass->set_color_attachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        subpass->set_depth_stencil_attachment(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        render_pass.add(subpass);

        auto first_subpass_dependency = subpass_dependency::make(VK_SUBPASS_EXTERNAL, 0);
        first_subpass_dependency->set_stage_mask(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        first_subpass_dependency->set_access_mask(VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        render_pass.add(first_subpass_dependency);

        auto second_subpass_dependency = subpass_dependency::make(0, VK_SUBPASS_EXTERNAL);
        second_subpass_dependency->set_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
        second_subpass_dependency->set_access_mask(VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT);
        render_pass.add(second_subpass_dependency);
    }
    
    auto depth_stencil = image::make(depth_format);
    if (!depth_stencil)
        return error::create_failed;

    depth_stencil->set_usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    depth_stencil->set_layout(VK_IMAGE_LAYOUT_UNDEFINED);
    depth_stencil->set_aspect_mask(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
    depth_stencil->set_component();

    render_target->on_create_attachments = [&]() -> VkAttachments {

        VkAttachments result;

        if (!depth_stencil->create(device, render_target->get_size()))
            return {};

        for (auto& backbuffer : render_target->get_backbuffers()) {

            VkImageViews attachments;

            attachments.push_back(backbuffer->get_view());
            attachments.push_back(depth_stencil->get_view());

            result.push_back(attachments);
        }

        return result;
    };

    render_target->on_destroy_attachments = [&]() {

        depth_stencil->destroy(); 
    };

    if (!render_pass.create(render_target->on_create_attachments(), { {}, render_target->get_size() }))
        return error::create_failed;

    render_target->add_callback(&render_pass);

    auto frame_count = render_target->get_frame_count();

    block block;
    if (!block.create(device, frame_count, device->get_graphics_queue().family))
        return error::create_failed;

    block.add_command([&](VkCommandBuffer cmd_buf) {

        render_pass.set_clear_color({ random(0.f, 1.f), random(0.f, 1.f), random(0.f, 1.f) });
        render_pass.process(cmd_buf, block.get_current_frame());
    });

    renderer simple_renderer;
    if (!simple_renderer.create(render_target->get_swapchain()))
        return error::create_failed;

    frame.add_run([&]() {

        input.handle_events();

        if (window.has_close_request())
            return frame.shut_down();

        if (window.has_resize_request())
            return window.handle_resize();

        if (window.iconified()) {

            frame.set_wait_for_events(true);
            return true;

        } else {

            if (frame.waiting_for_events())
                frame.set_wait_for_events(false);
        }

        auto frame_index = simple_renderer.begin_frame();
        if (!frame_index)
            return true;

        if (!block.process(*frame_index))
            return false;

        return simple_renderer.end_frame(block.get_buffers());
    });

    frame.add_run_end([&]() {

        render_pass.destroy();
        block.destroy();

        simple_renderer.destroy();

        render_target->destroy();
        depth_stencil->destroy();
    });

    return frame.run() ? 0 : error::aborted;
}
