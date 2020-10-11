// file      : liblava/asset/texture_loader.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/resource/texture.hpp>

namespace lava {

    texture::ptr load_texture(device_ptr device, file_format filename, texture_type type = texture_type::tex_2d);

    inline texture::ptr load_texture(device_ptr device, string_ref filename,
                                     VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, texture_type type = texture_type::tex_2d) {
        return load_texture(device, { filename, format }, type);
    }

    texture::ptr create_default_texture(device_ptr device, uv2 size = { 512, 512 });

} // namespace lava
