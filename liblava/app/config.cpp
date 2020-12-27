// file      : liblava/app/config.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#include <liblava/app/config.hpp>

namespace lava {

    constexpr name _window_file_ = "window.json";

    bool window_file() {
        return file_system::exists(_window_file_);
    }

    void to_json(json& j, window::state const& w) {
        j = json{ { _x_, w.x }, { _y_, w.y }, { _width_, w.width }, { _height_, w.height }, { _fullscreen_, w.fullscreen }, { _floating_, w.floating }, { _resizable_, w.resizable }, { _decorated_, w.decorated }, { _maximized_, w.maximized }, { _monitor_, w.monitor } };
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
        if (j.count(_monitor_))
            w.monitor = j.at(_monitor_).get<int>();
    }

    bool load_window_file(window::state& state, name save_name) {
        scope_data data;
        if (!load_file_data(_window_file_, data))
            return false;

        auto j = json::parse(data.ptr, data.ptr + data.size);

        if (!j.count(save_name))
            return false;

        log()->trace("load window {}", str(j.dump()));

        state = j[save_name];
        return true;
    }

    window::state::optional load_window_state(name save_name) {
        if (!window_file())
            return {};

        window::state window_state;
        if (!load_window_file(window_state, save_name))
            return {};

        return window_state;
    }

    void save_window_file(window::ref window) {
        auto state = window.get_state();
        auto index = window.get_save_name();

        json j;

        scope_data data;
        if (load_file_data(_window_file_, data)) {
            j = json::parse(data.ptr, data.ptr + data.size);

            json d;
            d[index] = state;

            j.merge_patch(d);
        } else {
            j[index] = state;
        }

        file file(str(_window_file_), true);
        if (!file.opened()) {
            log()->error("save window {}", str(j.dump()));
            return;
        }

        auto jString = j.dump(4);

        file.write(jString.data(), jString.size());

        log()->trace("save window {}", str(j.dump()));
    }

} // namespace lava
