// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/base/format.hpp>
#include <liblava/asset/texture.hpp>

#ifdef LIBLAVA_IMAGE_LOADING
#include <bitmap_image.hpp>
#include <selene/img/pixel/PixelTypeAliases.hpp>
#include <selene/img/typed/ImageView.hpp>
#include <selene/img_ops/ImageConversions.hpp>

#ifdef _WIN32
#    pragma warning(push, 4)
#    pragma warning(disable : 4458)
#    pragma warning(disable : 4100)
#    pragma warning(disable : 5054)
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

#include "stb_inc.hpp"
#endif

namespace lava
{

texture::ptr load_texture(device_ptr device, string_ref filename, VkFormat format, texture_type type) {
#ifdef LIBLAVA_IMAGE_LOADING
    auto supported = (format == VK_FORMAT_R8G8B8A8_UNORM) || 
        (device->get_features().textureCompressionBC && (format == VK_FORMAT_BC3_UNORM_BLOCK)) || 
        (device->get_features().textureCompressionASTC_LDR && (format == VK_FORMAT_ASTC_8x8_UNORM_BLOCK)) || 
        (device->get_features().textureCompressionETC2 && (format == VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK));
    if (!supported)
        return nullptr;

    auto use_gli = extension(str(filename), { "DDS", "KTX", "KMG" });
    auto use_stbi = false;

    if (!use_gli)
        use_stbi = extension(str(filename), { "JPG", "PNG", "TGA", "BMP", "PSD", "GIF", "HDR", "PIC" });

    if (!use_gli && !use_stbi)
        return nullptr;

    file file(str(filename));
    scope_data temp_data(file.get_size(), false);

    if (file.opened()) {
        if (!temp_data.allocate())
            return nullptr;

        if (file_error(file.read(temp_data.ptr)))
            return nullptr;
    }

    auto texture = make_texture();

    if (use_gli) {
        texture::layer::list layers;

        switch (type) {
        case texture_type::tex_2d: {
            gli::texture2d tex(file.opened() ? gli::load(temp_data.ptr, temp_data.size)
                                             : gli::load(filename));
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

            layers.push_back(layer);

            uv2 size = { tex[0].extent().x, tex[0].extent().y };
            if (!texture->create(device, size, format, layers, type))
                return nullptr;

            if (!texture->upload(tex.data(), tex.size()))
                return nullptr;

            break;
        }

        case texture_type::array: {
            gli::texture2d_array tex(file.opened() ? gli::load(temp_data.ptr, temp_data.size)
                                                   : gli::load(filename));
            assert(!tex.empty());
            if (tex.empty())
                return nullptr;

            auto layer_count = to_ui32(tex.layers());
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

            uv2 size = { tex[0].extent().x, tex[0].extent().y };
            if (!texture->create(device, size, format, layers, type))
                return nullptr;

            if (!texture->upload(tex.data(), tex.size()))
                return nullptr;

            break;
        }

        case texture_type::cube_map: {
            gli::texture_cube tex(file.opened() ? gli::load(temp_data.ptr, temp_data.size)
                                                : gli::load(filename));
            assert(!tex.empty());
            if (tex.empty())
                return nullptr;

            auto layer_count = to_ui32(tex.faces());
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

            uv2 size = { tex[0].extent().x, tex[0].extent().y };
            if (!texture->create(device, size, format, layers, type))
                return nullptr;

            if (!texture->upload(tex.data(), tex.size()))
                return nullptr;

            break;
        }
        }
    } else { // use_stbi
        i32 tex_width = 0, tex_height = 0;
        stbi_uc* data = nullptr;

        if (file.opened())
            data = stbi_load_from_memory((stbi_uc const*) temp_data.ptr, to_i32(temp_data.size),
                                         &tex_width, &tex_height, nullptr, STBI_rgb_alpha);
        else
            data = stbi_load(str(filename), &tex_width, &tex_height, nullptr, STBI_rgb_alpha);

        if (!data)
            return nullptr;

        uv2 size = { tex_width, tex_height };
        if (!texture->create(device, size, VK_FORMAT_R8G8B8A8_UNORM))
            return nullptr;

        const i32 tex_channels = 4;
        auto uploadSize = tex_width * tex_height * tex_channels * sizeof(char);
        auto result = texture->upload(data, uploadSize);

        stbi_image_free(data);

        if (!result)
            return nullptr;
    }
    return texture;
#else
    return nullptr;
#endif
}

texture::ptr create_default_texture(device_ptr device, uv2 size, VkFormat format) {
#ifdef LIBLAVA_IMAGE_LOADING
    auto texture = make_texture();

    if (!texture->create(device, size, format))
        return nullptr;

    bitmap_image image(size.x, size.y);
    checkered_pattern(64, 64, 255, 255, 255, image);

    image.bgr_to_rgb();

    sln::TypedLayout typed_layout((sln::PixelLength) size.x, (sln::PixelLength) size.y, (sln::Stride)(image.width() * image.bytes_per_pixel()));

    sln::ImageView<sln::PixelRGB_8u, sln::ImageModifiability::Mutable> img_rgb((uint8_t*) image.data(), typed_layout);

    sln::Image<sln::PixelRGBA_8u> const img_rgba = sln::convert_image<sln::PixelFormat::RGBA>(img_rgb, std::uint8_t{ 192 });

    if (!texture->upload(img_rgba.data(), img_rgba.total_bytes()))
        return nullptr;

    return texture;
#else
    return nullptr;
#endif
}

}