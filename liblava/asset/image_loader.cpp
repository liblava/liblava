/**
 * @file         liblava/asset/image_loader.cpp
 * @brief        Load image data from file and memory
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/asset/image_loader.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace lava {

//-----------------------------------------------------------------------------
image_loader::image_loader(string_ref filename) {
    file image_file(filename);
    unique_data data_guard(image_file.get_size(), data_mode::no_alloc);

    if (image_file.opened()) {
        if (!data_guard.allocate())
            return;

        if (file_error(image_file.read(data_guard.ptr)))
            return;
    }

    i32 tex_width, tex_height, tex_channels = 0;

    if (image_file.opened())
        data = as_ptr(stbi_load_from_memory((stbi_uc const*) data_guard.ptr,
                                            to_i32(data_guard.size),
                                            &tex_width, &tex_height,
                                            &tex_channels, STBI_rgb_alpha));
    else
        data = as_ptr(stbi_load(str(filename),
                                &tex_width, &tex_height,
                                &tex_channels, STBI_rgb_alpha));

    if (!data)
        return;

    dimensions = { tex_width, tex_height };
    channels = tex_channels;
}

//-----------------------------------------------------------------------------
image_loader::image_loader(cdata::ref image) {
    i32 tex_width, tex_height, tex_channels = 0;

    data = as_ptr(stbi_load_from_memory((stbi_uc const*) image.ptr,
                                        to_i32(image.size),
                                        &tex_width, &tex_height,
                                        &tex_channels, STBI_rgb_alpha));

    if (!data)
        return;

    dimensions = { tex_width, tex_height };
    channels = tex_channels;
}

//-----------------------------------------------------------------------------
image_loader::~image_loader() {
    if (data)
        stbi_image_free(data);
}

} // namespace lava
