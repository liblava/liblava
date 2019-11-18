// file      : liblava/app/app.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/app/app.hpp>

#include <imgui.h>

namespace lava {

app::app(argh::parser cmd_line) 
    : frame(cmd_line), window(get_config().app) {}

app::app(frame_config config) 
    : frame(config), window(config.app) {}

app::app(name config_app, argh::parser cmd_line, bool data_folder)
    : frame(frame_config(config_app, cmd_line, data_folder)), window(config_app) {}

bool app::setup() {

    if (!frame::ready())
        return false;

    if (!window.create())
        return false;

    device = create_device();
    if (!device)
        return false;

    if (!create_target())
        return false;

    if (!camera.create(device))
        return false;

    if (!create_gui())
        return false;

    if (!block.create(device, render_target->get_frame_count(), device->graphics_queue().family))
        return false;

    block.add_command([&](VkCommandBuffer cmd_buf) {

        auto current_frame = block.get_current_frame();

        staging.stage(cmd_buf, current_frame);

        forward_shading.get_render_pass()->process(cmd_buf, current_frame);
    });

    handle_input();
    handle_window();
    handle_update();
    handle_render();

    add_run_end([&]() {

        camera.destroy();

        destroy_gui();

        block.destroy();

        destroy_target();
    });

    add_run_once([&]() {

        return on_create ? on_create() : true;
    });

    return true;
}

bool app::create_gui() {

    gui.setup(window.get());
    if (!gui.create(device, render_target->get_frame_count(), forward_shading.get_render_pass()->get()))
        return false;

    forward_shading.get_render_pass()->add(gui.get_pipeline());

    fonts = texture::make();
    if (!gui.upload_fonts(fonts))
        return false;

    staging.add(fonts);

    return true;
}

void app::destroy_gui() {

    gui.destroy();
    fonts->destroy();
}

bool app::create_target() {

    render_target = lava::create_target(&window, device, v_sync);
    if (!render_target)
        return false;

    if (!forward_shading.create(render_target))
        return false;

    if (!plotter.create(render_target->get_swapchain()))
        return false;

    window.assign(&input);

    return on_create ? on_create() : true;
}

void app::destroy_target() {

    if (on_destroy)
        on_destroy();

    plotter.destroy();

    forward_shading.destroy();
    render_target->destroy();
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

        if (event.pressed(key::enter, mod::alt))
            window.set_fullscreen(!window.fullscreen());

        if (event.pressed(key::backspace, mod::alt))
            toggle_v_sync = true;

        if (camera.is_active())
            return camera.handle(event);

        return false;
    });

    input.mouse_button.listeners.add([&](mouse_button_event::ref event) {

        if (gui.want_capture_mouse())
            return false;

        if (camera.is_active())
            return camera.handle(event, input.get_mouse_position());

        return false;
    });

    input.scroll.listeners.add([&](scroll_event::ref event) {

        if (gui.want_capture_mouse())
            return false;

        if (camera.is_active())
            return camera.handle(event);

        return false;
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

        if (window.has_switch_mode_request() || toggle_v_sync) {

            device->wait_for_idle();

            destroy_target();
            destroy_gui();

            if (window.has_switch_mode_request() && !window.switch_mode())
                return false;

            if (toggle_v_sync) {

                v_sync = !v_sync;
                toggle_v_sync = false;
            }

            if (!create_target())
                return false;

            return create_gui();
        }

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

void app::draw_about(bool separator) const {

    if (separator)
        ImGui::Separator();

    ImGui::Text("%s %s", _liblava_, to_string(_version).c_str());

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("alt + enter = fullscreen\nalt + backspace = vsync\ntab = gui");

    if (v_sync_active())
        ImGui::Text("%.f fps (vsync)", ImGui::GetIO().Framerate);
    else
        ImGui::Text("%.f fps", ImGui::GetIO().Framerate);
}

} // lava
