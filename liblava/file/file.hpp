/**
 * @file         liblava/file/file.hpp
 * @brief        File access
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <fstream>
#include <liblava/core/data.hpp>

// fwd
struct PHYSFS_File;

namespace lava {

/// Zip file extension
constexpr name _zip_ = "zip";

/**
 * @brief File types
 */
enum class file_type : type {
    none = 0,
    fs,
    f_stream
};

/// File error result
constexpr i64 const file_error_result = -1;

/**
 * @brief Check file error result
 *
 * @param result    Result code to check
 *
 * @return true     Error result
 * @return false    No error result
 */
inline bool file_error(i64 result) {
    return result == file_error_result;
}

/**
 * @brief File modes
 */
enum class file_mode : type {
    read = 0,
    write
};

/**
 * @brief File
 */
struct file : no_copy_no_move {
    /**
     * @brief Construct a new file
     *
     * @param path    Name of file
     * @param mode    File mode
     */
    explicit file(name path = nullptr, file_mode mode = file_mode::read);

    /**
     * @brief Destroy the file
     */
    ~file();

    /**
     * @brief Open the file
     *
     * @param path      Name of file
     * @param mode      File mode
     *
     * @return true     Open was successful
     * @return false    Open failed
     */
    bool open(name path, file_mode mode = file_mode::read);

    /**
     * @brief Close the file
     */
    void close();

    /**
     * @brief Check if the file is opened
     *
     * @return true     File is opened
     * @return false    File is not opened
     */
    bool opened() const;

    /**
     * @brief Get the size of the file
     *
     * @return i64    File size
     */
    i64 get_size() const;

    /**
     * @brief Read data from file
     *
     * @param data    Data to read
     *
     * @return i64    File size
     */
    i64 read(data_ptr data) {
        return read(data, to_ui64(get_size()));
    }

    /**
     * @brief Read data from file (limited size)
     *
     * @param data    Data to read
     * @param size    File size
     *
     * @return i64    File size
     */
    i64 read(data_ptr data, ui64 size);

    /**
     * @brief Write data to file
     *
     * @param data    Data to write
     * @param size    File size
     *
     * @return i64    File size
     */
    i64 write(data_cptr data, ui64 size);

    /**
     * @brief Seek to position in the file
     *
     * @param position    Position to seek to
     *
     * @return i64        Current position
     */
    i64 seek(ui64 position);

    /**
     * @brief Get the current position in the file
     *
     * @return i64    Current position
     */
    i64 tell() const;

    /**
     * @brief Check if the file is in write mode
     *
     * @return true     File is writable
     * @return false    File is only readable
     */
    bool writable() const {
        return mode == file_mode::write;
    }

    /**
     * @brief Get the file type
     *
     * @return file_type    Type of file
     */
    file_type get_type() const {
        return type;
    }

    /**
     * @brief Get the path of the file
     *
     * @return name    File path
     */
    name get_path() const {
        return path;
    }

private:
    /// File type
    file_type type = file_type::none;

    /// File mode
    file_mode mode = file_mode::read;

    /// File path
    name path = nullptr;

    /// Physfs file handle
    PHYSFS_File* fs_file = nullptr;

    /// Std input file stream
    mutable std::ifstream i_stream;

    /// Std output file stream
    mutable std::ofstream o_stream;
};

} // namespace lava
