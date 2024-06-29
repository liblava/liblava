/**
 * @file         liblava/asset/load_image.cpp
 * @brief        Load image data from file and memory
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/asset/load_image.hpp"
#include "liblava/file/file.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace lava {

//-----------------------------------------------------------------------------
image_data::s_ptr load_image(string_ref filename) {
    file image_file(filename);
    u_data data_guard(image_file.get_size(), data::mode::no_alloc);

    if (image_file.opened()) {
        if (!data_guard.allocate())
            return nullptr;

        if (file_error(image_file.read(data_guard.addr)))
            return nullptr;
    }

    i32 tex_width, tex_height, tex_channels = 0;
    auto result = std::make_shared<image_data>();
    if (!result)
        return nullptr;

    if (image_file.opened())
        result->set_data(data::as_ptr(stbi_load_from_memory((stbi_uc const*)data_guard.addr,
                                                            to_i32(data_guard.size),
                                                            &tex_width, &tex_height,
                                                            &tex_channels, STBI_rgb_alpha)));
    else
        result->set_data(data::as_ptr(stbi_load(str(filename),
                                                &tex_width, &tex_height,
                                                &tex_channels, STBI_rgb_alpha)));

    if (!result->ready())
        return nullptr;

    result->dimensions = {tex_width, tex_height};
    result->channels = tex_channels;

    return result;
}

//-----------------------------------------------------------------------------
image_data::s_ptr load_image(c_data::ref data) {
    i32 tex_width, tex_height, tex_channels = 0;
    auto result = std::make_shared<image_data>();
    if (!result)
        return nullptr;

    result->set_data(data::as_ptr(stbi_load_from_memory((stbi_uc const*)data.addr,
                                                        to_i32(data.size),
                                                        &tex_width, &tex_height,
                                                        &tex_channels, STBI_rgb_alpha)));

    if (!result->ready())
        return nullptr;

    result->dimensions = {tex_width, tex_height};
    result->channels = tex_channels;

    return result;
}

} // namespace lava
