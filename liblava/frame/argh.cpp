/**
 * @file         liblava/frame/argh.cpp
 * @brief        Command line arguments
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/frame/argh.hpp"
#include "liblava/core/misc.hpp"
#include "liblava/util/log.hpp"

namespace lava {

//-----------------------------------------------------------------------------
void log_command_line(cmd_line cmd_line) {
    if (!cmd_line.pos_args().empty()) {
        for (auto const& pos_arg : cmd_line.pos_args())
            logger()->debug("cmd: {}", pos_arg);
    }

    if (!cmd_line.flags().empty()) {
        for (auto const& flag : cmd_line.flags())
            logger()->debug("cmd flag: {}", flag);
    }

    if (!cmd_line.params().empty()) {
        for (auto const& [key, value] : cmd_line.params())
            logger()->debug("cmd param: {} = {}", key, value);
    }
}

//-----------------------------------------------------------------------------
string get_cmd(cmd_line cmd_line,
               std::initializer_list<name const> names) {
    string result;

    if (auto arg = cmd_line(names)) {
        result = arg.str();
        remove_punctuation_marks(result);
    }

    return result;
}

} // namespace lava
