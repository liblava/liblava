/**
 * @file         liblava/frame/driver.cpp
 * @brief        Stage driver
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <iostream>
#include <liblava/frame/driver.hpp>
#include <liblava/util/utility.hpp>

namespace lava {

//-----------------------------------------------------------------------------
i32 driver::run(argh::parser cmd_line) {
    if (stages.empty()) {
        std::cerr << "no stages" << std::endl;
        return error::stages_empty;
    }

    if (cmd_line[{ "-ls", "--stages" }]) {
        for (auto& [id, stage] : stages)
            std::cout << id << ". " << stage->descr << std::endl;

        return to_i32(stages.size());
    }

    if (auto id = -1; cmd_line({ "-s", "--stage" }) >> id) {
        if (!stages.count(id)) {
            std::cerr << "stage " << id << " not found" << std::endl;
            return error::stage_not_found;
        }

        auto& stage = stages.at(id);
        std::cout << "stage " << id << " - " << stage->descr << std::endl;
        return stage->on_func(cmd_line);
    }

    if (on_run)
        return on_run(cmd_line);

    std::cerr << "run undefined" << std::endl;
    return error::undef_run;
}

//-----------------------------------------------------------------------------
stage::stage(ui32 id,
             name descr,
             func func)
: id(id), descr(descr), on_func(func) {
    assert((id != 0) && "stage id not defined.");
    driver::instance().add_stage(this);
}

} // namespace lava
