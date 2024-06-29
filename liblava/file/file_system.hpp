/**
 * @file         liblava/file/file_system.hpp
 * @brief        File system
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/core/version.hpp"
#include <filesystem>

namespace lava {

/**
 * @brief File system
 */
struct file_system : no_copy_no_move {
    /**
     * @brief Get the version
     * @return sem_version    Semantic version
     */
    sem_version get_version();

    /**
     * @brief Get the base directory
     * @return string    Base directory
     */
    string get_base_dir();

    /**
     * @brief Get the path relative to base directory
     * @param path       Path to add to base directory
     * @return string    Relative base directory path
     */
    string get_full_base_dir(string_ref path);

    /**
     * @brief Get the preferences directory
     * @return string    Preferences directory
     */
    string get_pref_dir();

    /**
     * @brief Get the resource directory
     * @return string    Resource directory
     */
    string get_res_dir();

    /**
     * @brief Mount path
     * @param path      Path to mount
     * @return Mount was successful or failed
     */
    bool mount(string_ref path);

    /**
     * @brief Mount base directory path
     * @param base_dir_path    Base directory path
     * @return Mount was successful or failed
     */
    bool mount_base(string_ref base_dir_path);

    /**
     * @brief Check if file exists
     * @param file      File to check
     * @return File exists or not found
     */
    bool exists(string_ref file);

    /**
     * @brief Get the real directory of file
     * @param file       Target file
     * @return string    Real directory of file
     */
    string get_real_dir(string_ref file);

    /**
     * @brief Enumerate files in directory
     * @param path            Target directory
     * @return string_list    List of files
     */
    string_list enumerate_files(string_ref path);

    /**
     * @brief Initialize the file system
     * @param argv_0    First command line argument
     * @param org       Organization name
     * @param app       Application name
     * @param ext       Extension name
     * @return Initialize was successful or failed
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
     * @brief Mount resource directories and files
     * @return string_list    List of mounted resources
     */
    string_list mount_res();

    /**
     * @brief Create a folder in the preferences directory
     * @param name    Name of folder (default: data)
     * @return Folder created or not
     */
    bool create_folder(string_ref name = "data");

    /**
     * @brief Clean preferences directory
     */
    void clean_pref_dir();

    /**
     * @brief Get the organization name
     * @return string_ref    Name of organization
     */
    string_ref get_org() const {
        return m_org;
    }

    /**
     * @brief Get the application name
     * @return string_ref    Name of application
     */
    string_ref get_app() const {
        return m_app;
    }

    /**
     * @brief Get the extension name
     * @return string_ref    Name of extension
     */
    string_ref get_ext() const {
        return m_ext;
    }

    /**
     * @brief Check if file system is ready
     * @return File system is ready or not
     */
    bool ready() const {
        return m_initialized;
    }

private:
    /// Initialized state
    bool m_initialized = false;

    /// Organization name
    string m_org;

    /// Application name
    string m_app;

    /// Extension name
    string m_ext;

    /// Path to resources
    string m_res_path;
};

} // namespace lava
