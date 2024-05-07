/**
 * @file         liblava/engine/engine.cpp
 * @brief        Engine
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/engine/engine.hpp"
#include "imgui.h"

namespace lava {

//-----------------------------------------------------------------------------
bool engine::setup() {
    handle_config();

    on_setup = [&]() {
        props.parse(get_cmd_line());

        if (!props.check()) {
            logger()->error("check props - make sure res is set correctly and props are available");

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

    producer.app = this;
    props.app = this;

    if (!headless)
        hud_menu();

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

        props.set_json(j_config[_props_]);
    };

    config_callback.on_save = [&]() {
        json j;
        j[config.id][_props_] = props.get_json();
        return j;
    };

    config_file.add(&config_callback);
}

//-----------------------------------------------------------------------------
void engine::hud_menu() {
    input.key.listeners.add([&](key_event::ref event) {
        if (!imgui.activated())
            return input_ignore;

        if (!config.handle_key_events)
            return input_ignore;

        if (event.pressed(key::grave_accent, mod::control)) {
            hud_active = !hud_active;
            return input_done;
        }

        return input_ignore;
    });

#if LAVA_DEBUG
    demo_layer = imgui.layers.add_inactive("imgui demo", []() {
        ImGui::ShowDemoWindow();
    });
#endif

    menu_layer = imgui.layers.add("hud", [&]() {
        if (!hud_active)
            return;

        if (!ImGui::BeginMainMenuBar())
            return;

        ImGui::Spacing();

        if (ImGui::MenuItem(run_time.paused
                                ? "PAUSE"
                                : "LIVE",
                            "Ctrl+Space",
                            !run_time.paused))
            run_time.paused = !run_time.paused;

        ImGui::Spacing();

        if (on_menu)
            on_menu();

        if (ImGui::BeginMenu("Layer")) {
            for (auto const& layer : imgui.layers.get_all()) {
                if (layer->get_id() == menu_layer) {
                    ImGui::MenuItem(layer->name.c_str(),
                                    "",
                                    &hud_active);
                } else {
                    ImGui::MenuItem(layer->name.c_str(),
                                    "",
                                    &layer->active);
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit",
                                "Ctrl+Q"))
                shut_down();

            ImGui::EndMenu();
        }

        ImGui::Spacing();

        ImGui::Text(str(get_fps_info()), ImGui::GetIO().Framerate);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s %s - v%s",
                              _liblava_,
                              str(version_string()),
                              str(sem_version_string()));

        ImGui::EndMainMenuBar();
    });
}

} // namespace lava
