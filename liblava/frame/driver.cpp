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
    if (cmd_line[{ "-ls", "--stages" }]) {
        for (auto& [id, stage] : stages)
            std::cout << id << " = " << stage->descr << std::endl;

        return to_i32(stages.size());
    }

    if (auto id = -1; cmd_line({ "-s", "--stage" }) >> id) {
        if (!stages.count(id)) {
            std::cerr << "stage " << id << " not found" << std::endl;
            return -1;
        }

        if (stages.count(id)) {
            auto stage = stages.at(id);
            std::cout << "stage " << id << " - " << stage->descr << std::endl;
            return stage->on_func(cmd_line);
        }
    }

    for (auto& [id, stage] : reverse(stages)) {
        std::cout << "stage " << id << " - " << stage->descr << std::endl;
        return stage->on_func(cmd_line);
    }

    std::cerr << "no stages" << std::endl;
    return -1;
}

//-----------------------------------------------------------------------------
stage::stage(ui32 id, name descr, func func)
: id(id), descr(descr), on_func(func) {
    driver::instance().add_stage(this);
}

} // namespace lava
