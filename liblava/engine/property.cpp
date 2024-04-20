/**
 * @file         liblava/engine/property.cpp
 * @brief        Props master
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/engine/property.hpp"
#include "liblava/base/base.hpp"
#include "liblava/engine/engine.hpp"
#include "liblava/file/file_system.hpp"

namespace lava {

//-----------------------------------------------------------------------------
void property::add(string_ref name,
                   string_ref filename) {
    map.emplace(name, filename);

    logger()->trace("prop: {} = {}", name, filename);
}

//-----------------------------------------------------------------------------
void property::remove(string_ref name) {
    if (!map.count(name))
        return;

    map.erase(name);
}

//-----------------------------------------------------------------------------
bool property::install(string_ref name,
                       string_ref filename) {
    add(name, filename);
    return load(name);
}

//-----------------------------------------------------------------------------
c_data property::operator()(string_ref name) {
    auto& prop = map.at(name);
    if (prop.data.addr)
        return {prop.data.addr, prop.data.size};

    if (!load_file_data(prop.filename, prop.data)) {
        logger()->error("prop get: {} = {}",
                        name, prop.filename);
        return {};
    }

    return {prop.data.addr, prop.data.size};
}

//-----------------------------------------------------------------------------
bool property::check() {
    auto result = true;

    for (auto& [name, prop] : map) {
        if (!context->fs.exists(prop.filename)) {
            logger()->warn("prop missing: {} = {}",
                           name, prop.filename);

            result = false;
        }
    }

    return result;
}

//-----------------------------------------------------------------------------
bool property::load(string_ref name) {
    auto& prop = map.at(name);
    if (prop.data.addr)
        prop.data = {}; // reload

    if (!load_file_data(prop.filename, prop.data)) {
        logger()->error("prop load: {} = {}",
                        name, prop.filename);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
bool property::load_all() {
    for (auto& [name, prop] : map) {
        if (!load_file_data(name, prop.data)) {
            logger()->error("prop load (all): {} = {}",
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

        auto filename = get_cmd(cmd_line, {cmd_arg.c_str()});
        if (!filename.empty()) {
            prop.filename = filename;

            logger()->debug("prop parse: {} = {}",
                            name, filename);
        }
    }
}

//-----------------------------------------------------------------------------
void property::set_json(json_ref j) {
    for (auto& [name, prop] : map) {
        if (j.count(name)) {
            string filename = j[name];

            if (prop.filename == filename)
                continue;

            prop.filename = filename;

            logger()->debug("prop config: {} = {}",
                            name, filename);
        }
    }
}

//-----------------------------------------------------------------------------
json property::get_json() const {
    json j;

    for (auto& [name, prop] : map)
        j[name] = prop.filename;

    return j;
}

} // namespace lava
