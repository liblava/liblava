/**
 * @file         liblava/asset/write_image.hpp
 * @brief        Write image data to file
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/resource/image.hpp"

namespace lava {

/**
 * @brief Write image data to png file
 * @param device      Vulkan device
 * @param image       Image to write
 * @param filename    File to write
 * @param swizzle     Swizzle data
 * @return Write was successful or failed
 */
bool write_image_png(device::ptr device,
                     image::ptr image,
                     string_ref filename,
                     bool swizzle);

} // namespace lava
