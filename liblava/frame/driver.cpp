/**
 * @file         liblava/frame/driver.cpp
 * @brief        Stage driver
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/frame/driver.hpp"
#include <iostream>
#include "liblava/core/misc.hpp"

namespace lava {

//-----------------------------------------------------------------------------
i32 driver::run(argh::parser cmd_line) {
    if (stages.empty()) {
        std::cerr << "no stages" << std::endl;
        return error::stages_empty;
    }

    if (cmd_line[{ "-ls", "--stages" }]) {
        for (auto& [id, stage] : stages)
            std::cout << id << ". " << stage->name << std::endl;

        return to_i32(stages.size());
    }

    if (auto id = -1; cmd_line({ "-s", "--stage" }) >> id) {
        if (!stages.count(id)) {
            std::cerr << "stage " << id << " not found" << std::endl;
            return error::stage_not_found;
        }

        auto& stage = stages.at(id);
        std::cout << "stage " << id << " - " << stage->name << std::endl;
        return stage->on_func(cmd_line);
    }

    if (!on_run) {
        std::cerr << "run undefined" << std::endl;
        return error::undef_run;
    }

    result result;

    do {
        result = on_run(cmd_line);
        if (result.driver < 0)
            return result.driver;

        if (stages.count(result.selected)) {
            auto& stage = stages.at(result.selected);

            std::cout << "stage " << result.selected
                      << " - " << stage->name << std::endl;

            auto stage_result = stage->on_func(cmd_line);
            if (stage_result < 0)
                return stage_result;

            result.driver = stage_result;

            if (result.selected != 0)
                std::cout << "stage driver" << std::endl;
        }
    } while (result.selected != 0);

    return result.driver;
}

//-----------------------------------------------------------------------------
stage::stage(ui32 id,
             string_ref name,
             func func)
: id(id), name(name), on_func(func) {
    assert((id != 0) && "stage id not defined.");
    driver::instance().add_stage(this);
}

} // namespace lava
