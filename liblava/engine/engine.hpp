/**
 * @file         liblava/engine/engine.hpp
 * @brief        Engine
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/app/app.hpp"
#include "liblava/engine/producer.hpp"
#include "liblava/engine/props.hpp"

namespace lava {

/// config
constexpr name _props_ = "props";

/**
 * @brief Engine
 */
struct engine : app {
    // Pointer to engine
    using ptr = engine*;

    /// App constructors
    using app::app;

    /**
     * @brief Set up the engine
     * @return Setup was successful or failed
     */
    bool setup() override;

    /// Props
    lava::props props;

    /// Producer
    lava::producer producer;

    /// Hud menu function
    using hud_menu_func = std::function<void()>;

    /// Function called on hud menu
    hud_menu_func on_menu;

    /// Hud active state
    bool hud_active = false;

private:
    /**
     * @brief Handle configuration file
     */
    void handle_config();

    /**
     * @brief Handle hud menu
     */
    void hud_menu();

    /// Hud menu id
    id m_menu_layer;

#if LAVA_DEBUG
    /// Demo id
    id m_demo_layer;
#endif

    /// Configuration file callback
    json_file::callback m_config_callback;
};

} // namespace lava
