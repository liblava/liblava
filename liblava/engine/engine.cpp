/**
 * @file         liblava/engine/engine.cpp
 * @brief        Engine
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/engine/engine.hpp"

namespace lava {

//-----------------------------------------------------------------------------
bool engine::setup() {
    handle_config();

    on_setup = [&]() {
        props.parse(get_cmd_line());

        if (!props.check()) {
            log()->error("check props - make sure res is set correctly and props are available");

            return false;
        }

        return true;
    };

    if (!app::setup())
        return false;

    add_run_once([&]() {
        props.unload_all();

        return run_continue;
    });

    add_run_end([&]() {
        producer.destroy();
    });

    producer.context = this;
    props.context = this;

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

        props.set_config(j_config[_props_]);
    };

    config_callback.on_save = [&]() {
        json j;
        j[config.id][_props_] = props.get_config();
        return j;
    };

    config_file.add(&config_callback);
}

} // namespace lava
