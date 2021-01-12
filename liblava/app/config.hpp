// file      : liblava/app/config.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/app/imgui.hpp>
#include <liblava/frame/window.hpp>
#include <liblava/resource/format.hpp>

namespace lava {

    struct app_config {
        bool save_window = true;
        bool handle_key_events = true;

        bool v_sync = false;
        surface_format_request surface;

        index physical_device = 0;

        imgui::font imgui_font;
    };

    window::state::optional load_window_state(name save_name);
    void save_window_file(window::ref window);

    void set_window_icon(window& window, string_ref icon_file = "icon.png");

} // namespace lava
