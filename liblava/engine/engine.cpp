/**
 * @file         liblava/engine/engine.cpp
 * @brief        Engine
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/engine/def.hpp>
#include <liblava/engine/engine.hpp>

namespace lava {

//-----------------------------------------------------------------------------
bool engine::setup() {
    handle_config();

    on_setup = [&]() {
        prop.parse(get_cmd_line());

        return prop.check();
    };

    if (!app::setup())
        return false;

    add_run_once([&]() {
        prop.unload_all();

        return run_continue;
    });

    return true;
}

//-----------------------------------------------------------------------------
void engine::handle_config() {
    config_callback.on_load = [&](json_ref j) {
        if (!j.count(config.id))
            return;

        auto j_config = j[config.id];
        if (!j_config.count(_props_))
            return;

        prop.set_config(j_config[_props_]);
    };

    config_callback.on_save = [&]() {
        json j;
        j[config.id][_props_] = prop.get_config();
        return j;
    };

    config_file.add(&config_callback);
}

} // namespace lava
