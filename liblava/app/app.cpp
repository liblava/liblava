// file      : liblava/app/app.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/app/app.hpp>

namespace lava {

app::app(argh::parser cmd_line) : frame(cmd_line), window(get_config().app) {}

app::app(frame_config config) : frame(config), window(config.app) {}

app::app(name config_app, argh::parser argh) : frame(frame_config(config_app, argh)), window(config_app) {}

bool app::setup() {

    if (!frame::ready())
        return false;

    if (!window.create())
        return false;

    window.assign(&input);

    device = create_device();
    if (!device)
        return false;

    render_target = create_target(&window, device);
    if (!render_target)
        return false;

    if (!forward_shading.create(render_target))
        return false;

    if (!camera.create(device))
        return false;

    auto frame_count = render_target->get_frame_count();

    gui.setup(window.get());
    if (!gui.create(device, frame_count, forward_shading.get_render_pass()->get()))
        return false;

    forward_shading.get_render_pass()->add(gui.get_pipeline());

    fonts = texture::make();
    if (!gui.upload_fonts(fonts))
        return false;

    staging.add(fonts);

    if (!block.create(device, frame_count, device->graphics_queue().family))
        return false;

    block.add_command([&](VkCommandBuffer cmd_buf) {

        auto current_frame = block.get_current_frame();

        staging.stage(cmd_buf, current_frame);

        forward_shading.get_render_pass()->process(cmd_buf, current_frame);
    });

    input.add(&gui);

    input.key.listeners.add([&](key_event::ref event) {

        if (gui.want_capture_mouse()) {

            camera.stop();
            return false;
        }

        if (event.pressed(key::tab))
            gui.toggle();

        if (event.pressed(key::escape))
            shut_down();

        return camera.handle(event);
    });

    input.mouse_button.listeners.add([&](mouse_button_event::ref event) {

        if (gui.want_capture_mouse())
            return false;

        return camera.handle(event, input.get_mouse_position());
    });

    input.scroll.listeners.add([&](scroll_event::ref event) {

        if (gui.want_capture_mouse())
            return false;

        return camera.handle(event);
    });

    if (!renderer.create(render_target->get_swapchain()))
        return false;

    last_time = now();

    add_run([&]() {

        input.handle_events();
        input.set_mouse_position(window.get_mouse_position());

        if (window.has_close_request())
            return shut_down();

        if (window.has_resize_request()) {

            camera.aspect_ratio = window.get_aspect_ratio();
            camera.update_projection();

            return window.handle_resize();
        }

        if (window.iconified()) {

            set_wait_for_events(true);
            return true;

        } else {

            if (waiting_for_events())
                set_wait_for_events(false);
        }

        auto delta = now() - last_time;
        camera.update_view(delta, input.get_mouse_position());
        last_time = now();

        auto frame_index = renderer.begin_frame();
        if (!frame_index)
            return true;

        if (!block.process(*frame_index))
            return false;

        return renderer.end_frame(block.get_buffers());
    });

    add_run_end([&]() {

        camera.destroy();

        input.remove(&gui);
        gui.destroy();

        fonts->destroy();

        block.destroy();
        forward_shading.destroy();

        renderer.destroy();
        render_target->destroy();
    });

    return true;
}

} // lava
