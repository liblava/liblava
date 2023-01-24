/**
 * @file         liblava/engine/property.cpp
 * @brief        Props master
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/base/base.hpp"
#include "liblava/engine/engine.hpp"
#include "liblava/engine/property.hpp"
#include "liblava/file/file_system.hpp"

namespace lava {

//-----------------------------------------------------------------------------
void property::add(string_ref name,
                   string_ref filename) {
    map.emplace(name, filename);

    log()->trace("prop: {} = {}", name, filename);
}

//-----------------------------------------------------------------------------
cdata property::operator()(string_ref name) {
    auto& prop = map.at(name);
    if (prop.data.ptr)
        return { prop.data.ptr, prop.data.size };

    if (!load_file_data(prop.filename, prop.data)) {
        log()->error("prop get: {} = {}",
                     name, prop.filename);
        return {};
    }

    return { prop.data.ptr, prop.data.size };
}

//-----------------------------------------------------------------------------
bool property::check() {
    auto result = true;

    for (auto& [name, prop] : map) {
        if (!context->fs.exists(prop.filename)) {
            log()->warn("prop missing: {} = {}",
                        name, prop.filename);

            result = false;
        }
    }

    return result;
}

//-----------------------------------------------------------------------------
bool property::load(string_ref name) {
    auto& prop = map.at(name);
    if (prop.data.ptr)
        prop.data = {}; // reload

    if (!load_file_data(prop.filename, prop.data)) {
        log()->error("prop load: {} = {}",
                     name, prop.filename);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
bool property::load_all() {
    for (auto& [name, prop] : map) {
        if (!load_file_data(name, prop.data)) {
            log()->error("prop load (all): {} = {}",
                         name, prop.filename);
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
void property::parse(cmd_line cmd_line) {
    for (auto& [name, prop] : map) {
        auto cmd_arg = "--" + name;

        if (auto value = cmd_line(cmd_arg)) {
            auto filename = value.str();
            remove_punctuation_marks(filename);

            prop.filename = filename;

            log()->debug("prop parse: {} = {}",
                         name, filename);
        }
    }
}

//-----------------------------------------------------------------------------
void property::set_config(json_ref j) {
    for (auto& [name, prop] : map) {
        if (j.count(name)) {
            auto filename = j[name];

            if (prop.filename == filename)
                continue;

            prop.filename = filename;

            log()->debug("prop config: {} = {}",
                         name, filename);
        }
    }
}

//-----------------------------------------------------------------------------
json property::get_config() const {
    json j;

    for (auto& [name, prop] : map)
        j[name] = prop.filename;

    return j;
}

} // namespace lava
