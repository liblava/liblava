/**
 * @file         liblava/file/json_file.cpp
 * @brief        Json file
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/file/json_file.hpp"
#include "liblava/core/misc.hpp"
#include "liblava/file/file.hpp"
#include "liblava/file/file_utils.hpp"

namespace lava {

//-----------------------------------------------------------------------------
json_file::json_file(string_ref path)
: m_path(path) {}

//-----------------------------------------------------------------------------
void json_file::add(callback* callback) {
    m_callbacks.push_back(callback);
}

//-----------------------------------------------------------------------------
void json_file::remove(callback* callback) {
    lava::remove(m_callbacks, callback);
}

//-----------------------------------------------------------------------------
bool json_file::load() {
    u_data data;
    if (!load_file_data(m_path, data))
        return false;

    if (data.size == 0)
        return false;

    if (!json::accept(data.addr, data.end()))
        return false;

    auto j = json::parse(data.addr, data.end());

    for (auto callback : m_callbacks)
        callback->on_load(j);

    return true;
}

//-----------------------------------------------------------------------------
bool json_file::save() {
    json j;

    u_data data;
    if (load_file_data(m_path, data) && (data.size > 0))
        if (json::accept(data.addr, data.end()))
            j = json::parse(data.addr, data.end());

    for (auto callback : m_callbacks) {
        auto d = callback->on_save();
        if (d.empty())
            continue;

        j.merge_patch(d);
    }

    file file(m_path, file_mode::write);
    if (!file.opened())
        return false;

    auto string = j.dump(4);

    file.write(string.data(), string.size());

    return true;
}

} // namespace lava
