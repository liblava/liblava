/**
 * @file         liblava/frame/argh.hpp
 * @brief        Command line arguments
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "argh.h"
#include "liblava/core/types.hpp"

namespace lava {

/// Command line
using cmd_line = argh::parser const&;

/**
 * @brief Log command line
 * @param cmd_line    Command line arguments
 */
void log_command_line(cmd_line cmd_line);

/**
 * @brief Get the value from command line arguments
 * @param cmd_line    Command line arguments
 * @param names       Argument names
 * @return string     Value of command line argument
 */
string get_cmd(cmd_line cmd_line,
               std::initializer_list<name const> names);

} // namespace lava
