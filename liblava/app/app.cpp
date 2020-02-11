// file      : liblava/app/app.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/app/app.hpp>
#include <liblava/app/def.hpp>

#include <imgui.h>

namespace lava {

app::app(frame_config config_) 
    : frame(config_), window(config_.app) {}

app::app(name config_app, argh::parser cmd_line)
    : frame(frame_config(config_app, cmd_line)), window(config_app) {}

void to_json(json& j, window::state const& w) {

    j = json{ { _x_, w.x }, { _y_, w.y }, { _width_, w.width }, { _height_, w.height }, 
              { _fullscreen_, w.fullscreen }, { _floating_, w.floating }, { _resizable_, w.resizable },
              { _decorated_, w.decorated }, { _maximized_, w.maximized } };
}

void from_json(json const& j, window::state& w) {

    if (j.count(_x_))
        w.x = j.at(_x_).get<int>();
    if (j.count(_y_))
        w.y = j.at(_y_).get<int>();
    if (j.count(_width_))
        w.width = j.at(_width_).get<int>();
    if (j.count(_height_))
        w.height = j.at(_height_).get<int>();
    if (j.count(_fullscreen_))
        w.fullscreen = j.at(_fullscreen_).get<bool>();
    if (j.count(_floating_))
        w.floating = j.at(_floating_).get<bool>();
    if (j.count(_resizable_))
        w.resizable = j.at(_resizable_).get<bool>();
    if (j.count(_decorated_))
        w.decorated = j.at(_decorated_).get<bool>();
    if (j.count(_maximized_))
        w.maximized = j.at(_maximized_).get<bool>();
}

bool has_window_file() {

    return file_system::exists(_window_file_);
}

bool load_window_file(window::state& state, name save_name) {

    scope_data data;
    if (!load_file_data(_window_file_, data))
        return false;

    auto j = json::parse({ data.ptr, data.size });

    if (!j.count(save_name))
        return false;

    log()->trace("load window {}", str(j.dump()));

    state = j[save_name];
    return true;
}

void save_window_file(window const& window) {

    window::state state = window.get_state();
    auto index = window.get_save_name();

    json j;

    scope_data data;
    if (load_file_data(_window_file_, data)) {

        j = json::parse({ data.ptr, data.size });

        json d;
        d[index] = state;

        j.merge_patch(d);
    }
    else {

        j[index] = state;
    }

    file file(str(_window_file_), true);
    if (!file.is_open()) {

        log()->error("save window {}", str(j.dump()));
        return;
    }

    auto jString = j.dump(4);

    file.write(jString.data(), jString.size());

    log()->trace("save window {}", str(j.dump()));
}

window::state::optional load_window_state(name save_name = _default_) {

    if (!has_window_file())
        return {};

    window::state window_state;
    if (!load_window_file(window_state, save_name))
        return {};

    return window_state;
}

void app::handle_config() {

    config_file.add(&config_callback);

    config_callback.on_load = [&](json& j) {

        if (j.count(_paused_))
            run_time.paused = j[_paused_];

        if (j.count(_speed_))
            run_time.speed = j[_speed_];

        if (j.count(_auto_save_))
            config.auto_save = j[_auto_save_];

        if (j.count(_save_interval_))
            config.save_interval = seconds((j[_save_interval_]));

        if (j.count(_auto_load_))
            config.auto_load = j[_auto_load_];

        if (j.count(_fixed_delta_))
            run_time.use_fix_delta = j[_fixed_delta_];

        if (j.count(_delta_))
            run_time.fix_delta = ms(j[_delta_]);

        if (j.count(_gui_))
            gui.set_active(j[_gui_]);

        if (j.count(_vsync_))
            config.vsync = j[_vsync_];
    };

    config_callback.on_save = [&](json& j) {

        j[_paused_] = run_time.paused;
        j[_speed_] = run_time.speed;
        j[_auto_save_] = config.auto_save;
        j[_save_interval_] = config.save_interval.count();
        j[_auto_load_] = config.auto_load;
        j[_fixed_delta_] = run_time.use_fix_delta;
        j[_delta_] = run_time.fix_delta.count();
        j[_gui_] = gui.is_active();
        j[_vsync_] = config.vsync;
    };

    config_file.load();
}

bool app::create_block() {

    if (!block.create(device, target->get_frame_count(), device->graphics_queue().family))
        return false;

    block.add_command([&](VkCommandBuffer cmd_buf) {

        auto current_frame = block.get_current_frame();

        staging.stage(cmd_buf, current_frame);

        shading.get_pass()->process(cmd_buf, current_frame);
    });

    return true;
}

bool app::setup() {

    if (!frame::ready())
        return false;

    handle_config();

    if (!window.create(load_window_state(window.get_save_name())))
        return false;

    set_window_icon();

    device = create_device();
    if (!device)
        return false;

    if (!create_target())
        return false;

    if (!camera.create(device))
        return false;

    if (!create_gui())
        return false;

    if (!create_block())
        return false;

    handle_input();
    handle_window();
    
    update();
    render();

    add_run_end([&]() {

        camera.destroy();

        destroy_gui();

        block.destroy();

        destroy_target();

        if (config.save_window)
            save_window_file(window);

        window.destroy();

        config_file.save();
        config_file.remove(&config_callback);
    });

    add_run_once([&]() {

        return on_create ? on_create() : true;
    });

    frame_counter = 0;

    return true;
}

bool app::create_gui() {

    gui_config.ini_file_dir = file_system::get_pref_dir();

    gui.setup(window.get(), gui_config);
    if (!gui.create(device, target->get_frame_count(), shading.get_vk_pass()))
        return false;

    shading.get_pass()->add(gui.get_pipeline());

    fonts = make_texture();
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

    target = lava::create_target(&window, device, config.vsync);
    if (!target)
        return false;

    if (!shading.create(target))
        return false;

    if (!plotter.create(target->get_swapchain()))
        return false;

    window.assign(&input);

    return on_create ? on_create() : true;
}

void app::destroy_target() {

    if (on_destroy)
        on_destroy();

    plotter.destroy();

    shading.destroy();
    target->destroy();
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
            return shut_down();

        if (event.pressed(key::enter, mod::alt)) {

            window.set_fullscreen(!window.fullscreen());
            return true;
        }

        if (event.pressed(key::backspace, mod::alt)) {

            toggle_vsync = true;
            return true;
        }

        if (event.pressed(key::space))
            run_time.paused = !run_time.paused;

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

        if (window.has_switch_mode_request() || toggle_vsync || target->must_reload()) {

            device->wait_for_idle();

            destroy_target();
            destroy_gui();

            if (window.has_switch_mode_request()) {

                if (config.save_window)
                    save_window_file(window);

                auto window_state = load_window_state(window.get_save_name());
                if (window_state)
                    window_state->fullscreen = !window.fullscreen();

                if (!window.switch_mode(window_state))
                    return false;

                set_window_icon();
            }

            if (toggle_vsync) {

                config.vsync = !config.vsync;
                toggle_vsync = false;
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

void app::update() {

    run_time.system = now();

    add_run([&]() {

        auto delta = ms(0);
        auto time = now();

        if (run_time.system != time) {

            delta = time - run_time.system;
            run_time.system = time;
        }

        run_time.delta = delta;

        if (!run_time.paused) {

            if (run_time.use_fix_delta)
                delta = run_time.fix_delta;

            delta = to_ms(to_sec(delta) * run_time.speed);
            run_time.current += delta;
        }
        else
            delta = ms(0);

        return on_update ? on_update(to_dt(delta)) : true;
    });
}

void app::render() {

    add_run([&]() {

        if (window.iconified()) {

            sleep(ms(1));
            return true;
        }

        auto frame_index = plotter.begin_frame();
        if (!frame_index)
            return true;

        frame_counter++;

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
        ImGui::SetTooltip("alt + enter = fullscreen\nalt + backspace = vsync\nspace = pause\ntab = gui");

    if (vsync_active())
        ImGui::Text("%.f fps (vsync)", ImGui::GetIO().Framerate);
    else
        ImGui::Text("%.f fps", ImGui::GetIO().Framerate);

    if (run_time.paused) {

        ImGui::SameLine();
        ImGui::TextUnformatted(_paused_);
    }
}

void app::set_window_icon() {

    scope_image icon("icon.png");
    if (icon.ready)
        window.set_icon(icon.data, icon.size);
}

} // lava
