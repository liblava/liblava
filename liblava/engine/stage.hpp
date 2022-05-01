/**
 * @file         liblava/engine/stage.hpp
 * @brief        Stage
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <argh.h>
#include <liblava/core/types.hpp>

namespace lava {

/**
 * @brief Stage
 */
struct stage {
    /// Map of stages
    using map = std::map<index, stage*>;

    /// Stage function
    using func = std::function<i32(argh::parser)>;

    /**
     * @brief Construct a new stage
     *
     * @param id       Stage id
     * @param descr    Stage description
     * @param func     Stage function
     */
    explicit stage(ui32 id, name descr, func func);

    /// Stage id
    index id = 0;

    /// Stage description
    string descr;

    /// Called on stage run
    func on_func;
};

} // namespace lava

/// Stage object
#define STAGE_OBJ stage_

/// Stage function
#define STAGE_FUNC _stage

/// String concatenation
#define STR_(n, m) n##m

/// String concatenation
#define STR(n, m) STR_(n, m)

/// Lava stage macro
#define LAVA_STAGE(ID, NAME) \
    i32 STR(STAGE_FUNC, ID)(argh::parser argh); \
    lava::stage STR(STAGE_OBJ, ID)(ID, NAME, ::STR(STAGE_FUNC, ID)); \
    i32 STR(STAGE_FUNC, ID)(argh::parser argh)
