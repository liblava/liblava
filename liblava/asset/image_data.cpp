/**
 * @file         liblava/asset/image_data.cpp
 * @brief        Load image data from file and memory
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <stb_image.h>
#include <liblava/asset/image_data.hpp>

namespace lava {

//-----------------------------------------------------------------------------
image_data::image_data(string_ref filename) {
    file image_file(str(filename));
    unique_data data_guard(image_file.get_size(), data_mode::no_alloc);

    if (image_file.opened()) {
        if (!data_guard.allocate())
            return;

        if (file_error(image_file.read(data_guard.ptr)))
            return;
    }

    i32 tex_width, tex_height, tex_channels = 0;

    if (image_file.opened())
        data = as_ptr(stbi_load_from_memory((stbi_uc const*) data_guard.ptr, to_i32(data_guard.size),
                                            &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha));
    else
        data = as_ptr(stbi_load(str(filename), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha));

    if (!data)
        return;

    dimensions = { tex_width, tex_height };
    channels = tex_channels;
}

//-----------------------------------------------------------------------------
image_data::image_data(cdata::ref image) {
    i32 tex_width, tex_height, tex_channels = 0;

    data = as_ptr(stbi_load_from_memory((stbi_uc const*) image.ptr, to_i32(image.size),
                                        &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha));

    if (!data)
        return;

    dimensions = { tex_width, tex_height };
    channels = tex_channels;
}

//-----------------------------------------------------------------------------
image_data::~image_data() {
    if (data)
        stbi_image_free(data);
}

} // namespace lava
