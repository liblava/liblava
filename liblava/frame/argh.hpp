/**
 * @file         liblava/frame/argh.hpp
 * @brief        Command line arguments
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "argh.h"
#include "liblava/util/log.hpp"

namespace lava {

/// Command line
using cmd_line = argh::parser const&;

/**
 * @brief Log command line
 * @param cmd_line    Command line arguments
 */
inline void log_command_line(cmd_line cmd_line) {
    if (!cmd_line.pos_args().empty()) {
        for (auto const& pos_arg : cmd_line.pos_args())
            log()->info("cmd {}", pos_arg);
    }

    if (!cmd_line.flags().empty()) {
        for (auto const& flag : cmd_line.flags())
            log()->info("cmd flag: {}", flag);
    }

    if (!cmd_line.params().empty()) {
        for (auto const& [key, value] : cmd_line.params())
            log()->info("cmd param: {} = {}", key, value);
    }
}

} // namespace lava
