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

    input.key_listeners.add([&](key_event::ref event) {

        if (event.key == GLFW_KEY_ESCAPE && event.action == GLFW_PRESS)
            frame.shut_down();
    });

    frame.add_run([&]() {

        handle_events(input);

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

    input.key_listeners.add([&](key_event::ref event) {

        if (event.key == GLFW_KEY_ESCAPE && event.action == GLFW_PRESS)
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

        VkCommandPoolCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = device->get_graphics_queue().family,
        };
        if (!device->vkCreateCommandPool(&create_info, &cmd_pool))
            return false;

        VkCommandBufferAllocateInfo alloc_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = cmd_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = frame_count,
        };
        if (!device->vkAllocateCommandBuffers(&alloc_info, cmd_bufs.data()))
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

        handle_events(input);

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

    input.key_listeners.add([&](key_event::ref event) {

        if (event.key == GLFW_KEY_ESCAPE && event.action == GLFW_PRESS)
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

        handle_events(input);

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
