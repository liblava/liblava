/**
 * @file         liblava/app/config.cpp
 * @brief        Application configuration
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/app/config.hpp"
#include "liblava/app/app.hpp"
#include "liblava/app/icon.hpp"
#include "liblava/asset/image_loader.hpp"

namespace lava {

/**
 * @brief Window state to Json
 * @param j    Json object
 * @param w    Window state
 */
void to_json(json& j, window::state::ref w) {
    j = json{ { _x_, w.x },
              { _y_, w.y },
              { _width_, w.width },
              { _height_, w.height },
              { _fullscreen_, w.fullscreen },
              { _floating_, w.floating },
              { _resizable_, w.resizable },
              { _decorated_, w.decorated },
              { _maximized_, w.maximized },
              { _monitor_, w.monitor } };
}

/**
 * @brief Json to window state
 * @param j    Json object
 * @param w    Window state
 */
void from_json(json_ref j, window::state& w) {
    if (j.count(_x_))
        w.x = j[_x_];
    if (j.count(_y_))
        w.y = j[_y_];
    if (j.count(_width_))
        w.width = j[_width_];
    if (j.count(_height_))
        w.height = j[_height_];
    if (j.count(_fullscreen_))
        w.fullscreen = j[_fullscreen_];
    if (j.count(_floating_))
        w.floating = j[_floating_];
    if (j.count(_resizable_))
        w.resizable = j[_resizable_];
    if (j.count(_decorated_))
        w.decorated = j[_decorated_];
    if (j.count(_maximized_))
        w.maximized = j[_maximized_];
    if (j.count(_monitor_))
        w.monitor = j[_monitor_];
}

//-----------------------------------------------------------------------------
void app_config::set_json(json_ref j) {
    if (j.count(_app_)) {
        auto j_app = j[_app_];

        if (j_app.count(_paused_))
            context->run_time.paused = j_app[_paused_];

        if (j_app.count(_speed_))
            context->run_time.speed = j_app[_speed_];

        if (j_app.count(_delta_))
            context->run_time.fix_delta = ms(j_app[_delta_]);

        if (j_app.count(_imgui_))
            context->imgui.set_active(j_app[_imgui_]);

        if (j_app.count(_v_sync_))
            v_sync = j_app[_v_sync_];

        if (j_app.count(_fps_cap_))
            fps_cap = j_app[_fps_cap_];

        if (j_app.count(_physical_device_))
            physical_device = j_app[_physical_device_];
    }

    if (j.count(_window_))
        window_state = j[_window_];
}

//-----------------------------------------------------------------------------
json app_config::get_json() const {
    json j;

    j[_app_][_paused_] = context->run_time.paused;
    j[_app_][_speed_] = context->run_time.speed;
    j[_app_][_delta_] = context->run_time.fix_delta.count();
    j[_app_][_imgui_] = context->imgui.activated();
    j[_app_][_v_sync_] = v_sync;
    j[_app_][_fps_cap_] = fps_cap;

    j[_app_][_physical_device_] = physical_device;

    if (window_state.has_value() && save_window)
        j[_window_] = window_state.value();

    return j;
}

//-----------------------------------------------------------------------------
void app_config::update_window_state() {
    window_state = context->window.get_state();
}

//-----------------------------------------------------------------------------
void set_window_icon(window& window, string_ref icon_file) {
    image_loader icon(icon_file);
    if (icon.ready()) {
        window.set_icon(icon.get(), icon.get_dimensions());
    } else {
        image_loader default_icon({ icon_png, icon_png_len });
        window.set_icon(default_icon.get(), default_icon.get_dimensions());
    }
}

} // namespace lava
