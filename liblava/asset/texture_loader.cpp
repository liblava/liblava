// file      : liblava/asset/texture_loader.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#include <liblava/asset/texture_loader.hpp>
#include <liblava/file.hpp>
#include <liblava/resource/format.hpp>

#ifdef _WIN32
#    pragma warning(push, 4)
#    pragma warning(disable : 4458)
#    pragma warning(disable : 4100)
#    pragma warning(disable : 5054)
#    pragma warning(disable : 4244)
#else
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wignored-qualifiers"
#    pragma GCC diagnostic ignored "-Wunused-variable"
#    pragma GCC diagnostic ignored "-Wtype-limits"
#    pragma GCC diagnostic ignored "-Wempty-body"
#    pragma GCC diagnostic ignored "-Wunused-result"
#endif

#include <gli/gli.hpp>

#ifdef _WIN32
#    pragma warning(pop)
#else
#    pragma GCC diagnostic pop
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace lava {

    texture::ptr create_gli_texture_2d(device_ptr device, file const& file, VkFormat format, scope_data const& temp_data) {
        gli::texture2d tex(file.opened() ? gli::load(temp_data.ptr, temp_data.size)
                                         : gli::load(file.get_path()));
        assert(!tex.empty());
        if (tex.empty())
            return nullptr;

        auto mip_levels = to_ui32(tex.levels());

        texture::layer layer;

        for (auto m = 0u; m < mip_levels; ++m) {
            texture::mip_level level;
            level.extent = { tex[m].extent().x, tex[m].extent().y };
            level.size = to_ui32(tex[m].size());

            layer.levels.push_back(level);
        }

        texture::layer::list layers;
        layers.push_back(layer);

        auto texture = make_texture();

        uv2 size = { tex[0].extent().x, tex[0].extent().y };
        if (!texture->create(device, size, format, layers, texture_type::tex_2d))
            return nullptr;

        if (!texture->upload(tex.data(), tex.size()))
            return nullptr;

        return texture;
    }

    template<typename T>
    texture::layer::list create_layer_list(T const& tex, ui32 layer_count) {
        texture::layer::list layers;

        auto mip_levels = to_ui32(tex.levels());

        for (auto i = 0u; i < layer_count; ++i) {
            texture::layer layer;

            for (auto m = 0u; m < mip_levels; ++m) {
                texture::mip_level level;
                level.extent = { tex[i][m].extent().x, tex[i][m].extent().y };
                level.size = to_ui32(tex[i][m].size());

                layer.levels.push_back(level);
            }

            layers.push_back(layer);
        }

        return layers;
    }

    texture::ptr create_gli_texture_array(device_ptr device, file const& file, VkFormat format, scope_data const& temp_data) {
        gli::texture2d_array tex(file.opened() ? gli::load(temp_data.ptr, temp_data.size)
                                               : gli::load(file.get_path()));
        assert(!tex.empty());
        if (tex.empty())
            return nullptr;

        auto layers = create_layer_list(tex, to_ui32(tex.layers()));

        auto texture = make_texture();

        uv2 size = { tex[0].extent().x, tex[0].extent().y };
        if (!texture->create(device, size, format, layers, texture_type::array))
            return nullptr;

        if (!texture->upload(tex.data(), tex.size()))
            return nullptr;

        return texture;
    }

    texture::ptr create_gli_texture_cube_map(device_ptr device, file const& file, VkFormat format, scope_data const& temp_data) {
        gli::texture_cube tex(file.opened() ? gli::load(temp_data.ptr, temp_data.size)
                                            : gli::load(file.get_path()));
        assert(!tex.empty());
        if (tex.empty())
            return nullptr;

        auto layers = create_layer_list(tex, to_ui32(tex.faces()));

        auto texture = make_texture();

        uv2 size = { tex[0].extent().x, tex[0].extent().y };
        if (!texture->create(device, size, format, layers, texture_type::cube_map))
            return nullptr;

        if (!texture->upload(tex.data(), tex.size()))
            return nullptr;

        return texture;
    }

    texture::ptr create_stbi_texture(device_ptr device, file const& file, scope_data const& temp_data) {
        i32 tex_width = 0, tex_height = 0;
        stbi_uc* data = nullptr;

        if (file.opened())
            data = stbi_load_from_memory((stbi_uc const*) temp_data.ptr, to_i32(temp_data.size),
                                         &tex_width, &tex_height, nullptr, STBI_rgb_alpha);
        else
            data = stbi_load(str(file.get_path()), &tex_width, &tex_height, nullptr, STBI_rgb_alpha);

        if (!data)
            return nullptr;

        auto texture = make_texture();

        uv2 size = { tex_width, tex_height };
        if (!texture->create(device, size, VK_FORMAT_R8G8B8A8_UNORM))
            return nullptr;

        auto const tex_channels = 4;
        auto uploadSize = tex_width * tex_height * tex_channels * sizeof(char);
        auto result = texture->upload(data, uploadSize);

        stbi_image_free(data);

        if (!result)
            return nullptr;

        return texture;
    }

} // namespace lava

lava::texture::ptr lava::load_texture(device_ptr device, file_format filename, texture_type type) {
    auto supported = (filename.format == VK_FORMAT_R8G8B8A8_UNORM) || (device->get_features().textureCompressionBC && (filename.format == VK_FORMAT_BC3_UNORM_BLOCK)) || (device->get_features().textureCompressionASTC_LDR && (filename.format == VK_FORMAT_ASTC_8x8_UNORM_BLOCK)) || (device->get_features().textureCompressionETC2 && (filename.format == VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK));
    if (!supported)
        return nullptr;

    auto use_gli = extension(str(filename.path), { "DDS", "KTX", "KMG" });
    auto use_stbi = false;

    if (!use_gli)
        use_stbi = extension(str(filename.path), { "JPG", "PNG", "TGA", "BMP", "PSD", "GIF", "HDR", "PIC" });

    if (!use_gli && !use_stbi)
        return nullptr;

    file file(str(filename.path));
    scope_data temp_data(file.get_size(), false);

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
            return create_gli_texture_2d(device, file, filename.format, temp_data);
        }

        case texture_type::array: {
            return create_gli_texture_array(device, file, filename.format, temp_data);
        }

        case texture_type::cube_map: {
            return create_gli_texture_cube_map(device, file, filename.format, temp_data);
        }
        }
    } else {
        return create_stbi_texture(device, file, temp_data);
    }

    return nullptr;
}

lava::texture::ptr lava::create_default_texture(device_ptr device, uv2 size, v3 color, r32 alpha) {
    auto result = make_texture();

    auto format = VK_FORMAT_R8G8B8A8_UNORM;
    if (!result->create(device, size, format))
        return nullptr;

    i32 block_size = format_block_size(format);
    scope_data data(size.s * size.t * block_size);
    memset(data.ptr, 0, data.size);

    ui32 color_r = 255 * color.r;
    ui32 color_g = 255 * color.g;
    ui32 color_b = 255 * color.b;
    ui32 color_a = 255 * alpha;

    for (auto y = 0u; y < size.t; ++y) {
        for (auto x = 0u; x < size.s; ++x) {
            auto const index = (x * block_size) + (y * size.t * block_size);
            if ((y % 128 < 64 && x % 128 < 64) || (y % 128 >= 64 && x % 128 >= 64)) {
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
