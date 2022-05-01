/**
 * @file         liblava/engine/stage.cpp
 * @brief        Stage
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/engine/driver.hpp>
#include <liblava/engine/stage.hpp>

namespace lava {

//-----------------------------------------------------------------------------
stage::stage(ui32 id, name descr, func func)
: id(id), descr(descr), on_func(func) {
    driver::instance().add_stage(this);
}

} // namespace lava
