/**
 * @file         liblava/file/json_file.hpp
 * @brief        Json file
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/file/json.hpp"

namespace lava {

/**
 * @brief Json file
 */
struct json_file {
    /**
     * @brief Construct a new json file
     * @param path    Name of file
     */
    explicit json_file(string_ref path = "config.json");

    /**
     * @brief Json file callback
     */
    struct callback {
        /// List of callbacks
        using list = std::vector<callback*>;

        /// Load function
        using load_func = std::function<void(json_ref)>;

        /// Called on load
        load_func on_load;

        /// Save function
        using save_func = std::function<json()>;

        /// Called on save
        save_func on_save;
    };

    /**
     * @brief Add callback to json file
     * @param callback    Callback to add
     */
    void add(callback* callback);

    /**
     * @brief Remove callback from json file
     * @param callback    Callback to remove
     */
    void remove(callback* callback);

    /**
     * @brief Clear all callbacks
     */
    void clear() {
        m_callbacks.clear();
    }

    /**
     * @brief Set path of the json file
     * @param value    Name of file
     */
    void set(string_ref value) {
        m_path = value;
    }

    /**
     * @brief Get path of the json file
     * @return name    Name of file
     */
    string_ref get() const {
        return m_path;
    }

    /**
     * @brief Load the json file
     * @return Load was successful or failed
     */
    bool load();

    /**
     * @brief Save the json file
     * @return Save was successful or failed
     */
    bool save();

private:
    /// Name of file
    string m_path;

    /// List of callbacks
    callback::list m_callbacks;
};

} // namespace lava
