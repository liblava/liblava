// file      : liblava/file/json_file.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/types.hpp>
#include <nlohmann/json.hpp>

namespace lava {

    constexpr name _config_file_ = "config.json";

    using json = nlohmann::json;

    struct json_file {
        explicit json_file(name path = _config_file_);

        struct callback {
            using list = std::vector<callback*>;

            using func = std::function<void(json&)>;
            func on_load;
            func on_save;
        };

        void add(callback* callback);
        void remove(callback* callback);

        void set(name value) {
            path = value;
        }
        name get() const {
            return str(path);
        }

        bool load();
        bool save();

    private:
        string path;
        callback::list callbacks;
    };

} // namespace lava
