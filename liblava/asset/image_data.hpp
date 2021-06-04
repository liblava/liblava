/**
 * @file liblava/asset/image_data.hpp
 * @brief Load image data from file
 * @authors Lava Block OÜ and contributors
 * @copyright Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/core/math.hpp>
#include <liblava/file/file.hpp>

namespace lava {

/**
 * @brief Load image data from file
 */
struct image_data {
    /**
     * @brief Construct a new image data
     * 
     * @param filename File to load
     */
    explicit image_data(string_ref filename);

    /**
     * @brief Destroy the image data
     */
    ~image_data();

    /// Data is ready
    bool ready = false;

    /// Pointer to data
    data_ptr data = nullptr;

    /// Dimensions
    uv2 size = uv2(0, 0);

    /// Number of channels
    ui32 channels = 0;

private:
    /// Image file
    file image_file;

    /// File data
    unique_data file_data;
};

} // namespace lava
