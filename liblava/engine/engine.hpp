/**
 * @file         liblava/engine/engine.hpp
 * @brief        Engine
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/app/app.hpp>
#include <liblava/engine/producer.hpp>
#include <liblava/engine/property.hpp>

namespace lava {

/**
 * @brief Engine
 */
struct engine : app {
    /// App constructors
    using app::app;

    /**
     * @brief Setup the engine
     *
     * @return true     Setup was successful
     * @return false    Setup failed
     */
    bool setup() override;

    /// Property master
    property prop;

    /// Producer
    producer_t producer;

private:
    /**
     * @brief Handle configuration file
     */
    void handle_config();

    /// Configuration file callback
    json_file::callback config_callback;
};

} // namespace lava
