// file      : liblava/app/app.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/app/app.hpp>

namespace lava {

app::app(argh::parser cmd_line) 
    : frame(cmd_line), window(get_config().app) {}

app::app(frame_config config) 
    : frame(config), window(config.app) {}

app::app(name config_app, argh::parser argh) 
    : frame(frame_config(config_app, argh)), window(config_app) {}

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

    if (!plotter.create(render_target->get_swapchain()))
        return false;

    handle_input();
    handle_window();
    handle_update();
    handle_render();

    add_run_end([&]() {

        camera.destroy();

        gui.destroy();
        fonts->destroy();

        block.destroy();
        forward_shading.destroy();

        plotter.destroy();
        render_target->destroy();
    });

    return true;
}

void app::handle_input() {

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

    add_run([&]() {

        input.handle_events();
        input.set_mouse_position(window.get_mouse_position());

        return true;
    });

    add_run_end([&]() {

        input.remove(&gui);
    });
}

void app::handle_window() {

    add_run([&]() {

        if (window.has_close_request())
            return shut_down();

        if (window.has_resize_request()) {

            camera.aspect_ratio = window.get_aspect_ratio();
            camera.update_projection();

            return window.handle_resize();
        }

        return true;
    });
}

void app::handle_update() {

    run_time.system = now();

    add_run([&]() {

        auto delta = milliseconds(0);
        auto time = now();

        if (run_time.system != time) {

            delta = time - run_time.system;
            run_time.system = time;
        }

        if (run_time.use_fix_delta)
            delta = run_time.fix_delta;

        run_time.delta = delta;
        run_time.current += delta;

        camera.update_view(delta, input.get_mouse_position());

        return on_update ? on_update(delta) : true;
    });
}

void app::handle_render() {

    add_run([&]() {

        if (window.iconified()) {

            sleep(milliseconds(1));
            return true;
        }

        auto frame_index = plotter.begin_frame();
        if (!frame_index)
            return true;

        if (!block.process(*frame_index))
            return false;

        return plotter.end_frame(block.get_buffers());
    });
}

} // lava
