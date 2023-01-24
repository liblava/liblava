/**
 * @file         liblava/asset/load_texture.cpp
 * @brief        Load texture from file
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/asset/load_texture.hpp"
#include "liblava/file.hpp"
#include "liblava/resource/format.hpp"

#ifdef _WIN32
    #pragma warning(push, 4)
    #pragma warning(disable : 4458)
    #pragma warning(disable : 4100)
    #pragma warning(disable : 5054)
    #pragma warning(disable : 4244)
    #pragma warning(disable : 4189)
#else
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wignored-qualifiers"
    #pragma GCC diagnostic ignored "-Wunused-variable"
    #pragma GCC diagnostic ignored "-Wtype-limits"
    #pragma GCC diagnostic ignored "-Wempty-body"
    #pragma GCC diagnostic ignored "-Wunused-result"
    #pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#endif

#include "gli/gli.hpp"

#ifdef _WIN32
    #pragma warning(pop)
#else
    #pragma GCC diagnostic pop
#endif

#include "stb_image.h"

namespace lava {

/**
 * @brief Create a gli 2D texture
 * @param device           Vulkan device
 * @param file             File to load
 * @param format           Format of texture
 * @param temp_data        Data of texture
 * @return texture::ptr    Loaded texture
 */
texture::ptr create_gli_texture_2d(device_p device,
                                   file::ref file,
                                   VkFormat format,
                                   unique_data::ref temp_data) {
    gli::texture2d tex(file.opened() ? gli::load(temp_data.ptr, temp_data.size)
                                     : gli::load(file.get_path()));
    LAVA_ASSERT(!tex.empty());
    if (tex.empty())
        return nullptr;

    auto mip_levels = to_ui32(tex.levels());

    texture::layer layer;

    for (auto m = 0u; m < mip_levels; ++m) {
        texture::mip_level level;
        level.extent = { tex[m].extent().x,
                         tex[m].extent().y };
        level.size = to_ui32(tex[m].size());

        layer.levels.push_back(level);
    }

    texture::layer::list layers;
    layers.push_back(layer);

    auto texture = texture::make();

    uv2 const size = { tex[0].extent().x,
                       tex[0].extent().y };
    if (!texture->create(device,
                         size,
                         format,
                         layers,
                         texture_type::tex_2d))
        return nullptr;

    if (!texture->upload(tex.data(), tex.size()))
        return nullptr;

    return texture;
}

/**
 * @brief Create a layer list for a texture
 * @param tex                      Target texture
 * @param layer_count              Number of layers
 * @return texture::layer::list    List of texture layers
 */
texture::layer::list create_layer_list(auto const& tex, ui32 layer_count) {
    texture::layer::list layers;

    auto const mip_levels = to_ui32(tex.levels());

    for (auto i = 0u; i < layer_count; ++i) {
        texture::layer layer;

        for (auto m = 0u; m < mip_levels; ++m) {
            texture::mip_level level;
            level.extent = { tex[i][m].extent().x,
                             tex[i][m].extent().y };
            level.size = to_ui32(tex[i][m].size());

            layer.levels.push_back(level);
        }

        layers.push_back(layer);
    }

    return layers;
}

/**
 * @brief Create a gli array texture
 * @param device           Vulkan device
 * @param file             File to load
 * @param format           Format of texture
 * @param temp_data        Data of texture
 * @return texture::ptr    Loaded texture
 */
texture::ptr create_gli_texture_array(device_p device,
                                      file::ref file,
                                      VkFormat format,
                                      unique_data::ref temp_data) {
    gli::texture2d_array tex(file.opened() ? gli::load(temp_data.ptr, temp_data.size)
                                           : gli::load(file.get_path()));
    LAVA_ASSERT(!tex.empty());
    if (tex.empty())
        return nullptr;

    auto layers = create_layer_list(tex, to_ui32(tex.layers()));

    auto texture = texture::make();

    uv2 const size = { tex[0].extent().x,
                       tex[0].extent().y };
    if (!texture->create(device,
                         size,
                         format,
                         layers,
                         texture_type::array))
        return nullptr;

    if (!texture->upload(tex.data(), tex.size()))
        return nullptr;

    return texture;
}

/**
 * @brief Create a gli cube map texture
 * @param device           Vulkan device
 * @param file             File to load
 * @param format           Format of texture
 * @param temp_data        Data of texture
 * @return texture::ptr    Loaded texture
 */
texture::ptr create_gli_texture_cube_map(device_p device,
                                         file::ref file,
                                         VkFormat format,
                                         unique_data::ref temp_data) {
    gli::texture_cube tex(file.opened() ? gli::load(temp_data.ptr, temp_data.size)
                                        : gli::load(file.get_path()));
    LAVA_ASSERT(!tex.empty());
    if (tex.empty())
        return nullptr;

    auto layers = create_layer_list(tex, to_ui32(tex.faces()));

    auto texture = texture::make();

    uv2 const size = { tex[0].extent().x,
                       tex[0].extent().y };
    if (!texture->create(device,
                         size,
                         format,
                         layers,
                         texture_type::cube_map))
        return nullptr;

    if (!texture->upload(tex.data(), tex.size()))
        return nullptr;

    return texture;
}

/**
 * @brief Create a stbi texture
 * @param device           Vulkan device
 * @param file             File to load
 * @param temp_data        Data of texture
 * @return texture::ptr    Loaded texture
 */
texture::ptr create_stbi_texture(device_p device,
                                 file::ref file,
                                 unique_data::ref temp_data) {
    i32 tex_width = 0, tex_height = 0;
    stbi_uc* data = nullptr;

    if (file.opened())
        data = stbi_load_from_memory((stbi_uc const*) temp_data.ptr,
                                     to_i32(temp_data.size),
                                     &tex_width,
                                     &tex_height,
                                     nullptr,
                                     STBI_rgb_alpha);
    else
        data = stbi_load(str(file.get_path()),
                         &tex_width,
                         &tex_height,
                         nullptr,
                         STBI_rgb_alpha);

    if (!data)
        return nullptr;

    auto texture = texture::make();

    uv2 const size = { tex_width, tex_height };
    auto const font_format = VK_FORMAT_R8G8B8A8_SRGB;
    if (!texture->create(device, size, font_format))
        return nullptr;

    auto const uploadSize = tex_width * tex_height * format_block_size(font_format);
    auto result = texture->upload(data, uploadSize);

    stbi_image_free(data);

    if (!result)
        return nullptr;

    return texture;
}

//-----------------------------------------------------------------------------
texture::ptr load_texture(device_p device,
                          file_format file_format,
                          texture_type type) {
    auto use_gli = extension(file_format.path,
                             { "DDS", "KTX", "KMG" });
    auto use_stbi = false;

    if (!use_gli)
        use_stbi = extension(file_format.path,
                             { "JPG", "PNG", "TGA", "BMP", "PSD", "GIF", "HDR", "PIC" });

    if (!use_gli && !use_stbi)
        return nullptr;

    file file(file_format.path);
    unique_data temp_data(file.get_size(), data_mode::no_alloc);

    if (file.opened()) {
        if (!temp_data.allocate())
            return nullptr;

        if (file_error(file.read(temp_data.ptr)))
            return nullptr;
    }

    if (use_gli) {
        texture::layer::list layers;

        switch (type) {
        case texture_type::tex_2d: {
            return create_gli_texture_2d(device,
                                         file,
                                         file_format.format,
                                         temp_data);
        }

        case texture_type::array: {
            return create_gli_texture_array(device,
                                            file,
                                            file_format.format,
                                            temp_data);
        }

        case texture_type::cube_map: {
            return create_gli_texture_cube_map(device,
                                               file,
                                               file_format.format,
                                               temp_data);
        }

        case texture_type::none: {
            return nullptr;
        }
        }
    } else {
        return create_stbi_texture(device,
                                   file,
                                   temp_data);
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
texture::ptr create_default_texture(device_p device,
                                    uv2 size,
                                    v3 color,
                                    r32 alpha) {
    auto result = texture::make();

    auto const format = VK_FORMAT_R8G8B8A8_UNORM;
    if (!result->create(device, size, format))
        return nullptr;

    i32 const block_size = format_block_size(format);
    unique_data data(size.x * size.y * block_size);
    memset(data.ptr, 0, data.size);

    ui32 const color_r = 255 * color.r;
    ui32 const color_g = 255 * color.g;
    ui32 const color_b = 255 * color.b;
    ui32 const color_a = 255 * alpha;

    for (auto y = 0u; y < size.y; ++y) {
        for (auto x = 0u; x < size.x; ++x) {
            auto const index = (x * block_size)
                               + (y * size.y * block_size);
            if (((y % 128 < 64) && (x % 128 < 64))
                || ((y % 128 >= 64) && (x % 128 >= 64))) {
                data.ptr[index] = color_r;
                data.ptr[index + 1] = color_g;
                data.ptr[index + 2] = color_b;
            }

            data.ptr[index + 3] = color_a;
        }
    }

    if (!result->upload(data.ptr, data.size))
        return nullptr;

    return result;
}

} // namespace lava
