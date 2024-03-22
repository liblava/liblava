/**
 * @file         liblava/file/file.hpp
 * @brief        File access
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/core/data.hpp"
#include <fstream>

// fwd
struct PHYSFS_File;

namespace lava {

/**
 * @brief File types
 */
enum class file_type : index {
    none = 0,
    fs,
    f_stream
};

/// File error result
constexpr i64 const file_error_result = undef;

/**
 * @brief Check file error result
 * @param result    Result code to check
 * @return Error result or okay
 */
inline bool file_error(i64 result) {
    return result == file_error_result;
}

/**
 * @brief File modes
 */
enum class file_mode : index {
    read = 0,
    write
};

/**
 * @brief File
 */
struct file : no_copy_no_move {
    /// Reference to file
    using ref = file const&;

    /**
     * @brief Construct a new file
     * @param path    Name of file
     * @param mode    File mode
     */
    explicit file(string_ref path = "",
                  file_mode mode = file_mode::read);

    /**
     * @brief Destroy the file
     */
    ~file();

    /**
     * @brief Open the file
     * @param path      Name of file
     * @param mode      File mode
     * @return Open was successful or failed
     */
    bool open(string_ref path,
              file_mode mode = file_mode::read);

    /**
     * @brief Close the file
     */
    void close();

    /**
     * @brief Check if the file is opened
     * @return File is opened or not
     */
    bool opened() const;

    /**
     * @brief Get the size of the file
     * @return i64    File size
     */
    i64 get_size() const;

    /**
     * @brief Read data from file
     * @param data    Data to read
     * @return i64    File size
     */
    i64 read(data_ptr data) {
        return read(data,
                    to_ui64(get_size()));
    }

    /**
     * @brief Read data from file (limited size)
     * @param data    Data to read
     * @param size    File size
     * @return i64    File size
     */
    i64 read(data_ptr data, ui64 size);

    /**
     * @brief Write data to file
     * @param data    Data to write
     * @param size    File size
     * @return i64    File size
     */
    i64 write(data_cptr data, ui64 size);

    /**
     * @brief Seek to position in the file
     * @param position    Position to seek to
     * @return i64        Current position
     */
    i64 seek(ui64 position);

    /**
     * @brief Get the current position in the file
     * @return i64    Current position
     */
    i64 tell() const;

    /**
     * @brief Check if the file is in write mode
     * @return File is writable or only readable
     */
    bool writable() const {
        return mode == file_mode::write;
    }

    /**
     * @brief Get the file type
     * @return file_type    Type of file
     */
    file_type get_type() const {
        return type;
    }

    /**
     * @brief Get the path of the file
     * @return name    File path
     */
    string_ref get_path() const {
        return path;
    }

private:
    /// File type
    file_type type = file_type::none;

    /// File mode
    file_mode mode = file_mode::read;

    /// File path
    string path;

    /// Physfs file handle
    PHYSFS_File* fs_file = nullptr;

    /// Std input file stream
    mutable std::ifstream i_stream;

    /// Std output file stream
    mutable std::ofstream o_stream;
};

} // namespace lava
