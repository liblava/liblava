// file      : liblava/app/app.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#include <imgui.h>
#include <liblava/app/app.hpp>
#include <liblava/app/def.hpp>
#include <liblava/base/debug_utils.hpp>

namespace lava {

    app::app(frame_config config)
    : frame(config), window(config.app) {}

    app::app(name name, argh::parser cmd_line)
    : frame(frame_config(name, cmd_line)), window(name) {}

    void app::handle_config() {
        config_file.add(&config_callback);

        config_callback.on_load = [&](json& j) {
            if (j.count(_paused_))
                run_time.paused = j[_paused_];

            if (j.count(_speed_))
                run_time.speed = j[_speed_];

            if (j.count(_fixed_delta_))
                run_time.use_fix_delta = j[_fixed_delta_];

            if (j.count(_delta_))
                run_time.fix_delta = ms(j[_delta_]);

            if (j.count(_imgui_))
                imgui.set_active(j[_imgui_]);

            if (j.count(_v_sync_))
                config.v_sync = j[_v_sync_];

            if (j.count(_physical_device_))
                config.physical_device = j[_physical_device_];
        };

        config_callback.on_save = [&](json& j) {
            j[_paused_] = run_time.paused;
            j[_speed_] = run_time.speed;
            j[_fixed_delta_] = run_time.use_fix_delta;
            j[_delta_] = run_time.fix_delta.count();
            j[_imgui_] = imgui.activated();
            j[_v_sync_] = config.v_sync;
            j[_physical_device_] = config.physical_device;
        };

        config_file.load();
    }

    bool app::create_block() {
        if (!block.create(device, target->get_frame_count(), device->graphics_queue().family))
            return false;

        block_command = block.add_cmd([&](VkCommandBuffer cmd_buf) {
            scoped_label block_label(cmd_buf, _lava_block_, { default_color, 1.f });

            auto current_frame = block.get_current_frame();

            {
                scoped_label stage_label(cmd_buf, _lava_texture_staging_, { 0.f, 0.13f, 0.4f, 1.f });
                staging.stage(cmd_buf, current_frame);
            }

            if (on_process)
                on_process(cmd_buf, current_frame);

            shading.get_pass()->process(cmd_buf, current_frame);
        });

        return true;
    }

    bool app::setup() {
        if (!frame::ready())
            return false;

        log()->debug("physfs {}", str(to_string(file_system::get_version())));

        if (!file_system::instance().initialize(str(get_cmd_line()[0]), get_config().org, get_config().app, get_config().ext)) {
            log()->error("init file system");
            return false;
        }

        file_system::instance().mount_res();

        handle_config();

        auto& cmd_line = get_cmd_line();
        cmd_line({ "-vs", "--v_sync" }) >> config.v_sync;
        cmd_line({ "-pd", "--physical_device" }) >> config.physical_device;

        if (!window.create(load_window_state(window.get_save_name())))
            return false;

        set_window_icon(window);

        if (!device) {
            device = create_device(config.physical_device);
            if (!device)
                return false;
        }

        if (!create_target())
            return false;

        if (!camera.create(device))
            return false;

        camera.aspect_ratio = window.get_aspect_ratio();
        camera.update_projection();

        if (!create_imgui())
            return false;

        if (!create_block())
            return false;

        handle_input();
        handle_window();

        update();
        render();

        add_run_end([&]() {
            camera.destroy();

            destroy_imgui();

            block.destroy();

            destroy_target();

            if (config.save_window)
                save_window_file(window);

            window.destroy();

            config_file.save();
            config_file.remove(&config_callback);

            file_system::instance().terminate();
        });

        add_run_once([&]() {
            return on_create ? on_create() : true;
        });

        frame_counter = 0;

        return true;
    }

    bool app::create_imgui() {
        if (config.imgui_font.file.empty()) {
            auto font_files = file_system::enumerate_files(_imgui_font_path_);
            if (!font_files.empty())
                config.imgui_font.file = fmt::format("{}{}", _imgui_font_path_, str(font_files.front()));
        }

        setup_imgui_font(imgui_config, config.imgui_font);

        imgui_config.ini_file_dir = file_system::get_pref_dir();

        imgui.setup(window.get(), imgui_config);
        if (!imgui.create(device, target->get_frame_count(), shading.get_vk_pass()))
            return false;

        shading.get_pass()->add(imgui.get_pipeline());

        imgui_fonts = make_texture();
        if (!imgui.upload_fonts(imgui_fonts))
            return false;

        staging.add(imgui_fonts);

        return true;
    }

    void app::destroy_imgui() {
        imgui.destroy();
        imgui_fonts->destroy();
    }

    bool app::create_target() {
        target = lava::create_target(&window, device, config.v_sync);
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
        input.add(&imgui);

        input.key.listeners.add([&](key_event::ref event) {
            if (imgui.capture_keyboard()) {
                camera.stop();
                return false;
            }

            if (config.handle_key_events) {
                if (event.pressed(key::tab))
                    imgui.toggle();

                if (event.pressed(key::escape))
                    return shut_down();

                if (event.pressed(key::enter, mod::alt)) {
                    window.set_fullscreen(!window.fullscreen());
                    return true;
                }

                if (event.pressed(key::backspace, mod::alt)) {
                    toggle_v_sync = true;
                    return true;
                }

                if (event.pressed(key::space))
                    run_time.paused = !run_time.paused;
            }

            if (camera.activated())
                return camera.handle(event);

            return false;
        });

        input.mouse_button.listeners.add([&](mouse_button_event::ref event) {
            if (imgui.capture_mouse())
                return false;

            if (camera.activated())
                return camera.handle(event, input.get_mouse_position());

            return false;
        });

        input.scroll.listeners.add([&](scroll_event::ref event) {
            if (imgui.capture_mouse())
                return false;

            if (camera.activated())
                return camera.handle(event);

            return false;
        });

        add_run([&]() {
            input.handle_events();
            input.set_mouse_position(window.get_mouse_position());

            return true;
        });

        add_run_end([&]() {
            input.remove(&imgui);
        });
    }

    void app::handle_window() {
        add_run([&]() {
            if (window.close_request())
                return shut_down();

            if (window.switch_mode_request() || toggle_v_sync || target->reload_request()) {
                device->wait_for_idle();

                destroy_target();
                destroy_imgui();

                if (window.switch_mode_request()) {
                    if (config.save_window)
                        save_window_file(window);

                    auto window_state = load_window_state(window.get_save_name());
                    if (window_state)
                        window_state->fullscreen = !window.fullscreen();

                    if (!window.switch_mode(window_state))
                        return false;

                    set_window_icon(window);
                }

                if (toggle_v_sync) {
                    config.v_sync = !config.v_sync;
                    toggle_v_sync = false;
                }

                if (!create_target())
                    return false;

                return create_imgui();
            }

            if (window.resize_request()) {
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
            auto dt = ms(0);
            auto time = now();

            if (run_time.system != time) {
                dt = time - run_time.system;
                run_time.system = time;
            }

            run_time.delta = dt;

            if (!run_time.paused) {
                if (run_time.use_fix_delta)
                    dt = run_time.fix_delta;

                dt = to_ms(to_sec(dt) * run_time.speed);
                run_time.current += dt;
            } else
                dt = ms(0);

            return on_update ? on_update(to_delta(dt)) : true;
        });
    }

    void app::render() {
        add_run([&]() {
            if (window.iconified()) {
                sleep(one_ms);
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
            ImGui::SetTooltip("alt + enter = fullscreen\nalt + backspace = v-sync\nspace = pause\ntab = gui");

        if (v_sync())
            ImGui::Text("%.f fps (v-sync)", ImGui::GetIO().Framerate);
        else
            ImGui::Text("%.f fps", ImGui::GetIO().Framerate);

        if (run_time.paused) {
            ImGui::SameLine();
            ImGui::TextUnformatted(_paused_);
        }
    }

} // namespace lava
