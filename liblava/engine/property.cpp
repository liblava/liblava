/**
 * @file         liblava/engine/property.cpp
 * @brief        Property master
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/base/base.hpp>
#include <liblava/engine/property.hpp>
#include <liblava/file/file_system.hpp>

namespace lava {

//-----------------------------------------------------------------------------
void property::add(string_ref name, string_ref filename) {
    map.emplace(name, filename);

    log()->trace("prop: {} = {}", str(name), str(filename));
}

//-----------------------------------------------------------------------------
cdata property::operator()(string_ref name) {
    auto& prop = map.at(name);
    if (prop.data.ptr != nullptr)
        return { prop.data.ptr, prop.data.size };

    if (!load_file_data(prop.filename, prop.data)) {
        log()->error("prop get: {} = {}", str(name), str(prop.filename));
        return {};
    }

    return { prop.data.ptr, prop.data.size };
}

//-----------------------------------------------------------------------------
bool property::check() {
    auto result = true;

    for (auto& [name, prop] : map) {
        if (!file_system::exists(str(prop.filename))) {
            log()->critical("prop check: {} = {}", str(name), str(prop.filename));

            result = false;
        }
    }

    return result;
}

//-----------------------------------------------------------------------------
bool property::load(string_ref name) {
    auto& prop = map.at(name);
    if (prop.data.ptr != nullptr)
        prop.data = {}; // reload

    if (!load_file_data(prop.filename, prop.data)) {
        log()->error("prop load: {} = {}", str(name), str(prop.filename));
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
bool property::load_all() {
    for (auto& [name, prop] : map) {
        if (!load_file_data(name, prop.data)) {
            log()->error("prop load (all): {} = {}", str(name), str(prop.filename));
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
            remove_chars(filename, _punctuation_marks_);

            prop.filename = filename;

            log()->debug("prop parse: {} = {}", str(name), str(filename));
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

            log()->debug("prop config: {} = {}", str(name), str(filename));
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
