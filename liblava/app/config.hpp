/**
 * @file         liblava/app/config.hpp
 * @brief        Application configuration
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/app/imgui.hpp>
#include <liblava/frame/window.hpp>
#include <liblava/resource/format.hpp>

namespace lava {

/**
 * @brief Application configuration
 */
struct app_config {
    /// Organization name
    name org = _liblava_;

    /// Preferred compression file format
    name ext = _zip_;

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
};

/**
 * @brief Load window state from file
 *
 * @param save_name    Name of the saved window
 *
 * @return window::state::optional    Window state if available
 */
window::state::optional load_window_state(name save_name);

/**
 * @brief Save window state to file
 *
 * @param window    Window to save
 */
void save_window_file(window::ref window);

/**
 * @brief Set the window icon
 *
 * @param window       Target window
 * @param icon_file    Icon file
 */
void set_window_icon(window& window, string_ref icon_file = "icon.png");

} // namespace lava
