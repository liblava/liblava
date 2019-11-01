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

        sleep(seconds(1));
        count++;

        log()->debug("{} - running {} sec", count, to_sec(frame.get_running_time()));

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

        if (event.pressed(key::escape))
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

        if (event.pressed(key::escape))
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

        if (!device->vkCreateCommandPool(device->graphics_queue().family, &cmd_pool))
            return false;

        if (!device->vkAllocateCommandBuffers(cmd_pool, frame_count, cmd_bufs.data()))
            return false;

        VkCommandBufferBeginInfo begin_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
        };

        VkClearColorValue clear_color = { lava::random(0.f, 1.f), lava::random(0.f, 1.f), lava::random(0.f, 1.f), 0.f };

        VkImageSubresourceRange image_range
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1,
        };

        for (auto i = 0u; i < frame_count; ++i) {

            auto cmd_buf = cmd_bufs[i];
            auto frame_image = render_target->get_image(i);

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

        if (event.pressed(key::escape))
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

    if (!block.create(device, frame_count, device->graphics_queue().family))
        return error::create_failed;

    block.add_command([&](VkCommandBuffer cmd_buf) {

        VkClearColorValue clear_color = { lava::random(0.f, 1.f), lava::random(0.f, 1.f), lava::random(0.f, 1.f), 0.f };

        VkImageSubresourceRange image_range
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1,
        };

        auto frame_image = render_target->get_image(block.get_current_frame());

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

        sleep(seconds(1));

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

        if (event.pressed(key::escape))
            frame.shut_down();
    });

    auto device = frame.create_device();
    if (!device)
        return error::create_failed;

    auto render_target = create_target(&window, device);
    if (!render_target)
        return error::create_failed;

    forward_shading forward_shading;
    if (!forward_shading.create(render_target))
        return error::create_failed;

    auto render_pass = forward_shading.get_render_pass();

    block block;
    if (!block.create(device, render_target->get_frame_count(), device->graphics_queue().family))
        return false;

    block.add_command([&](VkCommandBuffer cmd_buf) {

        render_pass->set_clear_color({ lava::random(0.f, 1.f), lava::random(0.f, 1.f), lava::random(0.f, 1.f) });
        render_pass->process(cmd_buf, block.get_current_frame());
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

        block.destroy();
        forward_shading.destroy();
        
        simple_renderer.destroy();
        render_target->destroy();
    });

    return frame.run() ? 0 : error::aborted;
}

#include <imgui.h>

LAVA_TEST(8, "imgui demo")
{
    frame frame(argh);
    if (!frame.ready())
        return error::not_ready;

    window window;
    if (!window.create())
        return error::create_failed;

    input input;
    window.assign(&input);

    auto device = frame.create_device();
    if (!device)
        return error::create_failed;

    auto render_target = create_target(&window, device);
    if (!render_target)
        return error::create_failed;

    auto frame_count = render_target->get_frame_count();

    forward_shading forward_shading;
    if (!forward_shading.create(render_target))
        return error::create_failed;

    auto render_pass = forward_shading.get_render_pass();

    gui gui(window.get());
    if (!gui.create(graphics_pipeline::make(device), frame_count))
        return error::create_failed;

    {
        auto gui_pipeline = gui.get_pipeline();

        if (!gui_pipeline->create(render_pass->get()))
            return error::create_failed;

        render_pass->add(gui_pipeline);
    }

    auto fonts = texture::make();
    if (!gui.upload_fonts(fonts))
        return error::create_failed;

    staging staging;
    staging.add(fonts);

    block block;
    if (!block.create(device, frame_count, device->get_graphics_queue().family))
        return false;

    block.add_command([&](VkCommandBuffer cmd_buf) {

        auto current_frame = block.get_current_frame();

        staging.stage(cmd_buf, current_frame);

        render_pass->process(cmd_buf, current_frame);
    });

    gui.on_draw = [&]() {

        ImGui::ShowDemoWindow();
    };

    input.add(&gui);

    input.key.listeners.add([&](key_event::ref event) {

        if (event.pressed(key::tab))
            gui.toggle();

        if (event.pressed(key::escape))
            frame.shut_down();
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

        input.remove(&gui);

        gui.destroy();
        fonts->destroy();

        block.destroy();
        forward_shading.destroy();

        simple_renderer.destroy();
        render_target->destroy();
    });

    return frame.run() ? 0 : error::aborted;
}
