/**
 * @file         liblava/engine/driver.hpp
 * @brief        Stage driver
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/engine/stage.hpp>

namespace lava {

/**
 * @brief Stage driver
 */
struct driver {
    /**
     * @brief Get driver singleton
     *
     * @return driver&    Stage driver
     */
    static driver& instance() {
        static driver singleton;
        return singleton;
    }

    /**
     * @brief Add a stage
     *
     * @param stage    Stage to add
     */
    void add_stage(stage* stage) {
        stages.emplace(stage->id, stage);
    }

    /**
     * @brief Get all stages
     *
     * @return stage::map const&    Map of stages
     */
    stage::map const& get_stages() const {
        return stages;
    }

    /**
     * @brief Run the driver
     *
     * @param cmd_line    Command line arguments
     *
     * @return i32        Result code
     */
    i32 run(argh::parser cmd_line = {});

private:
    /**
     * @brief Construct a new driver
     */
    driver() = default;

    /// Map of stages
    stage::map stages;
};

} // namespace lava
