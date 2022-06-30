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
     * @brief Get the version
     *
     * @return internal_version    Internal version
     */
    internal_version get_version();

    /**
     * @brief Get the base directory
     *
     * @return string    Base directory
     */
    string get_base_dir();

    /**
     * @brief Get the preferences directory
     *
     * @return name    Preferences directory
     */
    string get_pref_dir();

    /**
     * @brief Get the resource directory
     *
     * @return string    Resource directory
     */
    string get_res_dir();

    /**
     * @brief Mount path
     *
     * @param path      Path to mount
     *
     * @return true     Mount was successful
     * @return false    Mount failed
     */
    bool mount(string_ref path);

    /**
     * @brief Mount base directory path
     *
     * @param base_dir_path    Base directory path
     *
     * @return true            Mount was successful
     * @return false           Mount failed
     */
    bool mount_base(string_ref base_dir_path);

    /**
     * @brief Check if file exists
     *
     * @param file      File to check
     *
     * @return true     File exists
     * @return false    File not found
     */
    bool exists(string_ref file);

    /**
     * @brief Get the real directory of file
     *
     * @param file     Target file
     *
     * @return name    Real directory of file
     */
    string get_real_dir(string_ref file);

    /**
     * @brief Enumerate files in directory
     *
     * @param path            Target directory
     *
     * @return string_list    List of files
     */
    string_list enumerate_files(string_ref path);

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
    bool initialize(string_ref argv_0,
                    string_ref org,
                    string_ref app,
                    string_ref ext);

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
    bool create_folder(string_ref name = "data");

    /**
     * @brief Clean preferences directory
     */
    void clean_pref_dir();

    /**
     * @brief Get the organization name
     *
     * @return name    Name of organization
     */
    string_ref get_org() const {
        return org;
    }

    /**
     * @brief Get the application name
     *
     * @return name    Name of application
     */
    string_ref get_app() const {
        return app;
    }

    /**
     * @brief Get the extension name
     *
     * @return name    Name of extension
     */
    string_ref get_ext() const {
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
    /// Initialized state
    bool initialized = false;

    /// Organization name
    string org;

    /// Application name
    string app;

    /// Extension name
    string ext;

    /// Path to resources
    string res_path;
};

} // namespace lava
