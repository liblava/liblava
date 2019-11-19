// file      : liblava/app/app.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/frame.hpp>
#include <liblava/block.hpp>

#include <liblava/app/forward_shading.hpp>
#include <liblava/app/camera.hpp>
#include <liblava/app/gui.hpp>

namespace lava {

struct app : frame {

    explicit app(frame_config config);
    explicit app(name config_app, argh::parser cmd_line = {});

    bool setup();

    lava::window window;
    lava::input input;

    lava::forward_shading forward_shading;

    lava::gui gui;
    lava::camera camera;

    lava::staging staging;
    lava::block block;

    renderer plotter;
    device_ptr device = nullptr;

    lava::render_target::ptr render_target;

    lava::run_time run_time;

    using update_func = std::function<bool(milliseconds)>;
    update_func on_update;

    using create_func = std::function<bool()>;
    create_func on_create;

    using destroy_func = std::function<void()>;
    destroy_func on_destroy;

    bool vsync_active() const { return vsync; }

    void draw_about(bool separator = true) const;

    bool save_window = true;

    bool auto_load = true;
    bool auto_save = true;
    seconds save_interval{300};

    json_file config;

private:
    void handle_input();
    void handle_window();
    void handle_update();
    void handle_render();

    bool create_gui();
    void destroy_gui();

    bool create_target();
    void destroy_target();

    texture::ptr fonts;

    bool toggle_vsync = false;
    bool vsync = false;

    file_callback config_callback;
};

} // lava
