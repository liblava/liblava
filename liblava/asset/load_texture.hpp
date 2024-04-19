/**
 * @file         liblava/asset/load_texture.hpp
 * @brief        Load texture from file
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/resource/texture.hpp"

namespace lava {

/**
 * @brief Load texture from file
 * @param device             Vulkan device
 * @param file_format        File and format
 * @param type               Type of texture
 * @return texture::s_ptr    Loaded texture
 */
texture::s_ptr load_texture(device::ptr device,
                            file_format file_format,
                            texture_type type = texture_type::tex_2d);

/**
 * @brief Load texture from file with default format (sRGB)
 * @param device             Vulkan device
 * @param filename           File to load
 * @param format             Format of texture
 * @param type               Type of texture
 * @return texture::s_ptr    Loaded texture
 */
inline texture::s_ptr load_texture(device::ptr device,
                                   string_ref filename,
                                   VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
                                   texture_type type = texture_type::tex_2d) {
    return load_texture(device, {filename, format}, type);
}

/**
 * @brief Create a default texture with checkerboard pattern
 * @param device             Vulkan device
 * @param size               Size of texture
 * @param color              Color of texture
 * @param alpha              Alpha value of texture
 * @return texture::s_ptr    Loaded texture
 */
texture::s_ptr create_default_texture(device::ptr device,
                                      uv2 size = {512, 512},
                                      v3 color = v3(1.f),
                                      r32 alpha = 0.7529f);

} // namespace lava
