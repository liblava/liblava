/**
 * @file         liblava/file/file_system.hpp
 * @brief        File system
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <filesystem>
#include <liblava/core/version.hpp>
#include <liblava/util/log.hpp>

namespace lava {

/**
 * @brief File system
 */
struct file_system : no_copy_no_move {
    /**
     * @brief Get file system singleton
     *
     * @return file_system&    File system
     */
    static file_system& instance() {
        static file_system fs;
        return fs;
    }

    /**
     * @brief Get the version
     *
     * @return internal_version    Internal version
     */
    static internal_version get_version();

    /**
     * @brief Get the base directory
     *
     * @return string    Base directory
     */
    static string get_base_dir();

    /**
     * @brief Get the preferences directory
     *
     * @return name    Preferences directory
     */
    static string get_pref_dir();

    /**
     * @brief Get the resource directory
     *
     * @return string    Resource directory
     */
    static string get_res_dir();

    /**
     * @brief Mount path
     *
     * @param path      Path to mount
     *
     * @return true     Mount was successful
     * @return false    Mount failed
     */
    static bool mount(string_ref path);

    /**
     * @brief Mount base directory path
     *
     * @param base_dir_path    Base directory path
     *
     * @return true            Mount was successful
     * @return false           Mount failed
     */
    static bool mount(name base_dir_path);

    /**
     * @brief Check if file exists
     *
     * @param file      File to check
     *
     * @return true     File exists
     * @return false    File not found
     */
    static bool exists(name file);

    /**
     * @brief Get the real directory of file
     *
     * @param file     Target file
     *
     * @return name    Real directory of file
     */
    static name get_real_dir(name file);

    /**
     * @brief Enumerate files in directory
     *
     * @param path            Target directory
     *
     * @return string_list    List of files
     */
    static string_list enumerate_files(name path);

    /**
     * @brief Initialize the file system
     *
     * @param argv_0    First command line argument
     * @param org       Organization name
     * @param app       Application name
     * @param ext       Extension name
     *
     * @return true     Initialize was successful
     * @return false    Initialize failed
     */
    bool initialize(name argv_0,
                    name org,
                    name app,
                    name ext);

    /**
     * @brief Terminate the file system
     */
    void terminate();

    /**
     * @brief Mount resource directory
     *
     * @param log    Logger
     */
    void mount_res(logger log);

    /**
     * @brief Create a folder in the preferences directory (default: data)
     *
     * @param name      Name of folder
     *
     * @return true     Folder created
     * @return false    Folder not created
     */
    bool create_folder(name name = "data");

    /**
     * @brief Clean preferences directory
     */
    void clean_pref_dir();

    /**
     * @brief Get the organization name
     *
     * @return name    Name of organization
     */
    name get_org() const {
        return org;
    }

    /**
     * @brief Get the application name
     *
     * @return name    Name of application
     */
    name get_app() const {
        return app;
    }

    /**
     * @brief Get the extension name
     *
     * @return name    Name of extension
     */
    name get_ext() const {
        return ext;
    }

    /**
     * @brief Check if file system is ready
     *
     * @return true     File system is ready
     * @return false    File system is not ready
     */
    bool ready() const {
        return initialized;
    }

private:
    /**
     * @brief Construct a new file system
     */
    file_system() = default;

    /// Initialized state
    bool initialized = false;

    /// Organization name
    name org = nullptr;

    /// Application name
    name app = nullptr;

    /// Extension name
    name ext = nullptr;

    /// Path to resources
    string res_path;
};

} // namespace lava
