/**
 * @file         liblava-stage/tutorial.hpp
 * @brief        Tutorial stages
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <imgui.h>
#include <liblava/lava.hpp>

using namespace lava;

//-----------------------------------------------------------------------------
LAVA_STAGE(1, "frame") {
    frame frame(argh);

    return frame.ready() ? 0 : error::not_ready;
}

//-----------------------------------------------------------------------------
LAVA_STAGE(2, "run loop") {
    frame frame(argh);
    if (!frame.ready())
        return error::not_ready;

    ui32 count = 0;

    frame.add_run([&](id::ref run) {
        sleep(one_second);
        count++;

        log()->debug("{} - running {} sec",
                     count, frame.get_running_time_sec());

        if (count == 3)
            return frame.shut_down();

        return run_continue;
    });

    return frame.run();
}

//-----------------------------------------------------------------------------
LAVA_STAGE(3, "window input") {
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
            return frame.shut_down();

        return input_ignore;
    });

    frame.add_run([&](id::ref run) {
        input.handle_events();

        if (window.close_request())
            return frame.shut_down();

        return run_continue;
    });

    return frame.run();
}

//-----------------------------------------------------------------------------
LAVA_STAGE(4, "clear color") {
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
            return frame.shut_down();

        return input_ignore;
    });

    device_p device = frame.platform.create_device();
    if (!device)
        return error::create_failed;

    render_target::ptr render_target = create_target(&window, device);
    if (!render_target)
        return error::create_failed;

    renderer renderer;
    if (!renderer.create(render_target->get_swapchain()))
        return error::create_failed;

    ui32 frame_count = render_target->get_frame_count();

    VkCommandPool cmd_pool;
    VkCommandBuffers cmd_bufs(frame_count);

    auto build_cmd_bufs = [&]() {
        if (!device->vkCreateCommandPool(device->graphics_queue().family, &cmd_pool))
            return build_failed;

        if (!device->vkAllocateCommandBuffers(cmd_pool, frame_count, cmd_bufs.data()))
            return build_failed;

        VkCommandBufferBeginInfo const begin_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
        };

        VkClearColorValue const clear_color = {
            random(1.f), random(1.f), random(1.f), 0.f
        };

        VkImageSubresourceRange const image_range{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1,
        };

        for (auto i = 0u; i < frame_count; ++i) {
            VkCommandBuffer cmd_buf = cmd_bufs[i];
            VkImage frame_image = render_target->get_image(i);

            if (failed(device->call().vkBeginCommandBuffer(cmd_buf, &begin_info)))
                return build_failed;

            insert_image_memory_barrier(device,
                                        cmd_buf,
                                        frame_image,
                                        VK_ACCESS_MEMORY_READ_BIT,
                                        VK_ACCESS_TRANSFER_WRITE_BIT,
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                                        image_range);

            device->call().vkCmdClearColorImage(cmd_buf,
                                                frame_image,
                                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                &clear_color,
                                                1,
                                                &image_range);

            insert_image_memory_barrier(device,
                                        cmd_buf,
                                        frame_image,
                                        VK_ACCESS_TRANSFER_WRITE_BIT,
                                        VK_ACCESS_MEMORY_READ_BIT,
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                                        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                        image_range);

            if (failed(device->call().vkEndCommandBuffer(cmd_buf)))
                return build_failed;
        }

        return build_done;
    };

    auto clean_cmd_bufs = [&]() {
        device->vkFreeCommandBuffers(cmd_pool, frame_count, cmd_bufs.data());
        device->vkDestroyCommandPool(cmd_pool);
    };

    if (!build_cmd_bufs())
        return error::create_failed;

    render_target->on_swapchain_start = build_cmd_bufs;
    render_target->on_swapchain_stop = clean_cmd_bufs;

    frame.add_run([&](id::ref run) {
        input.handle_events();

        if (window.close_request())
            return frame.shut_down();

        if (window.resize_request())
            return window.handle_resize();

        optional_index current_frame = renderer.begin_frame();
        if (!current_frame.has_value())
            return run_continue;

        return renderer.end_frame({ cmd_bufs[*current_frame] });
    });

    frame.add_run_end([&]() {
        clean_cmd_bufs();

        renderer.destroy();
        render_target->destroy();
    });

    return frame.run();
}

//-----------------------------------------------------------------------------
LAVA_STAGE(5, "color block") {
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
            return frame.shut_down();

        return input_ignore;
    });

    device_p device = frame.platform.create().get();
    if (!device)
        return error::create_failed;

    render_target::ptr render_target = create_target(&window, device);
    if (!render_target)
        return error::create_failed;

    renderer renderer;
    if (!renderer.create(render_target->get_swapchain()))
        return error::create_failed;

    ui32 frame_count = render_target->get_frame_count();

    block block;

    if (!block.create(device, frame_count, device->graphics_queue().family))
        return error::create_failed;

    block.add_command([&](VkCommandBuffer cmd_buf) {
        VkClearColorValue const clear_color = {
            random(1.f), random(1.f), random(1.f), 0.f
        };

        VkImageSubresourceRange const image_range{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1,
        };

        VkImage frame_image = render_target->get_image(block.get_current_frame());

        insert_image_memory_barrier(device,
                                    cmd_buf,
                                    frame_image,
                                    VK_ACCESS_MEMORY_READ_BIT,
                                    VK_ACCESS_TRANSFER_WRITE_BIT,
                                    VK_IMAGE_LAYOUT_UNDEFINED,
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                                    image_range);

        device->call().vkCmdClearColorImage(cmd_buf,
                                            frame_image,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                            &clear_color,
                                            1,
                                            &image_range);

        insert_image_memory_barrier(device,
                                    cmd_buf,
                                    frame_image,
                                    VK_ACCESS_TRANSFER_WRITE_BIT,
                                    VK_ACCESS_MEMORY_READ_BIT,
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                                    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                    image_range);
    });

    frame.add_run([&](id::ref run) {
        input.handle_events();

        if (window.close_request())
            return frame.shut_down();

        if (window.resize_request())
            return window.handle_resize();

        optional_index current_frame = renderer.begin_frame();
        if (!current_frame.has_value())
            return run_continue;

        if (!block.process(*current_frame))
            return run_abort;

        return renderer.end_frame(block.get_buffers());
    });

    frame.add_run_end([&]() {
        block.destroy();

        renderer.destroy();
        render_target->destroy();
    });

    return frame.run();
}

//-----------------------------------------------------------------------------
LAVA_STAGE(6, "imgui demo") {
    app app("imgui demo", argh);
    if (!app.setup())
        return error::not_ready;

    app.imgui.on_draw = []() {
        ImGui::ShowDemoWindow();
    };

    return app.run();
}
