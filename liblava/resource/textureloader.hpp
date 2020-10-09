// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/asset/texture.hpp>

namespace lava {

    // Load a texture file from disk. Will return a valid mesh pointer if successful, or a nullptr if loading failed.
    texture::ptr load_texture(device_ptr device, string_ref filename, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, texture_type type = texture_type::tex_2d);

    // Create a default checkerboard 2D texture.
    texture::ptr create_checker_texture2d(device_ptr device, uv2 size, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);

} // namespace lava
