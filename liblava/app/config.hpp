/**
 * @file         liblava/app/config.hpp
 * @brief        Application configuration
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/app/imgui.hpp>
#include <liblava/frame/window.hpp>
#include <liblava/fwd.hpp>
#include <liblava/resource/format.hpp>

namespace lava {

/**
 * @brief Application configuration
 */
struct app_config : configurable {
    /// Application
    app* context = nullptr;

    /// Organization name
    name org = _liblava_;

    /// Preferred compression file format
    name ext = "zip";

    /// Save window state
    bool save_window = true;

    /// Handle key events
    bool handle_key_events = true;

    /// Activate V-Sync
    bool v_sync = false;

    /// Request surface formats
    surface_format_request surface;

    /// Physical device index
    index physical_device = 0;

    /// ImGui font settings
    imgui::font imgui_font;

    /// Identification
    string id = _default_;

    /**
     * @brief Set config
     * @param j    Json file
     */
    void set_config(json_ref j) override;

    /**
     * @brief Get config
     * @return json    Json file
     */
    json get_config() const override;

    /**
     * @brief Update window state
     */
    void update_window_state();

    /// Window state if available
    window::state::optional window_state;
};

/**
 * @brief Set the window icon
 * @param window       Target window
 * @param icon_file    Icon file
 */
void set_window_icon(window& window, string_ref icon_file = "icon.png");

} // namespace lava
