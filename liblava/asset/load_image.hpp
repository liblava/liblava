/**
 * @file         liblava/asset/load_image.hpp
 * @brief        Load image data from file and memory
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/resource/image.hpp"

namespace lava {

/**
 * @brief Load image data from file
 * @param filename              File to load
 * @return image_data::s_ptr    Loaded image
 */
image_data::s_ptr load_image(string_ref filename);

/**
 * @brief Load image data from memory
 * @param data                  Memory data to load
 * @return image_data::s_ptr    Loaded image
 */
image_data::s_ptr load_image(c_data::ref data);

} // namespace lava
