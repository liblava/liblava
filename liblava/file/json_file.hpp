/**
 * @file         liblava/file/json_file.hpp
 * @brief        Json file
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/core/types.hpp>
#include <nlohmann/json.hpp>

namespace lava {

/// Configuration json file
constexpr name _config_file_ = "config.json";

/// Json
using json = nlohmann::json;

/**
 * @brief Json file
 */
struct json_file {
    /**
     * @brief Construct a new json file
     * 
     * @param path    Name of file
     */
    explicit json_file(name path = _config_file_);

    /**
     * @brief Json file callback
     */
    struct callback {
        /// List of callbacks
        using list = std::vector<callback*>;

        /// Callback function
        using func = std::function<void(json&)>;

        /// Called on load
        func on_load;

        /// Called on save
        func on_save;
    };

    /**
     * @brief Add callback to json file
     * 
     * @param callback    Callback to add
     */
    void add(callback* callback);

    /**
     * @brief Remove callback from json file
     * 
     * @param callback    Callback to remove
     */
    void remove(callback* callback);

    /**
     * @brief Set path of the json file
     * 
     * @param value    Name of file
     */
    void set(name value) {
        path = value;
    }

    /**
     * @brief Get path of the json file
     * 
     * @return name    Name of file
     */
    name get() const {
        return str(path);
    }

    /**
     * @brief Load the json file
     * 
     * @return true     Load was successful
     * @return false    Load failed
     */
    bool load();

    /**
     * @brief Save the json file
     * 
     * @return true     Save was successful
     * @return false    Save failed
     */
    bool save();

private:
    /// Name of file
    string path;

    /// List of callbacks
    callback::list callbacks;
};

} // namespace lava
