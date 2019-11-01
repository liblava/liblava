// file      : liblava/resource/texture.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/resource/texture.hpp>
#include <liblava/resource/format.hpp>

#include <bitmap_image.hpp>
#include <selene/img/pixel/PixelTypeAliases.hpp>
#include <selene/img/typed/ImageView.hpp>
#include <selene/img_ops/ImageConversions.hpp>

#ifdef _WIN32
#pragma warning(push, 4)
#pragma warning(disable : 4458)
#pragma warning(disable : 4100)
#pragma warning(disable : 5054)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wempty-body"
#pragma GCC diagnostic ignored "-Wunused-result"
#endif

#include <gli/gli.hpp>

#ifdef _WIN32
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace lava {

bool texture::create(device* device, uv2 size, VkFormat format, layer::list const& layers_, texture_type type_) {

    layers = layers_;
    type = type_;

    if (layers.empty()) {

        layer layer;

        mip_level level;
        level.extent = size;

        layer.levels.push_back(level);
        layers.push_back(layer);
    }

    VkSamplerAddressMode sampler_address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    if (type == texture_type::array || type == texture_type::cube_map)
        sampler_address_mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    VkSamplerCreateInfo sampler_info
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = sampler_address_mode,
        .addressModeV = sampler_address_mode,
        .addressModeW = sampler_address_mode,
        .mipLodBias = 0.f,
        .anisotropyEnable = device->get_features().samplerAnisotropy,
        .maxAnisotropy = device->get_properties().limits.maxSamplerAnisotropy,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_NEVER,
        .minLod = 0.f,
        .maxLod = to_r32(layers.front().levels.size()),
        .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };


    if (!device->vkCreateSampler(&sampler_info, &sampler)) {

        log()->error("texture create vkCreateSampler failed");
        return false;
    }

    _image = image::make(format);

    if (type == texture_type::cube_map)
        _image->set_flags(VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);

    auto view_type = VK_IMAGE_VIEW_TYPE_2D;
    if (type == texture_type::array) {

        view_type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;

    } else if (type == texture_type::cube_map) {

        view_type = VK_IMAGE_VIEW_TYPE_CUBE;
    }

    _image->set_tiling(VK_IMAGE_TILING_LINEAR);
    _image->set_level_count(to_ui32(layers.front().levels.size()));
    _image->set_layer_count(to_ui32(layers.size()));
    _image->set_view_type(view_type);

    if (!_image->create(device, size, VMA_MEMORY_USAGE_GPU_ONLY)) {

        log()->error("texture create failed");
        return false;
    }

    descriptor.sampler = sampler;
    descriptor.imageView = _image->get_view();
    descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    return true;
}

void texture::destroy() {

    destroy_upload_buffer();

    device* device = nullptr;
    if (_image)
        device = _image->get_device();

    if (sampler) {

        if (device)
            device->vkDestroySampler(sampler);

        sampler = nullptr;
    }

    if (_image) {

        _image->destroy();
        _image = nullptr;
    }
}

void texture::destroy_upload_buffer() {

    upload_buffer = nullptr;
}

bool texture::upload(void const* data, size_t data_size) {

    upload_buffer = buffer::make();

    return upload_buffer->create(_image->get_device(), data, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, false, VMA_MEMORY_USAGE_CPU_TO_GPU);
}

bool texture::stage(VkCommandBuffer cmd_buffer) {

    if (!upload_buffer || !upload_buffer->is_valid()) {

        log()->error("texture stage failed");
        return false;
    }

    VkImageSubresourceRange subresource_range
    {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = to_ui32(layers.front().levels.size()),
        .baseArrayLayer = 0,
        .layerCount = to_ui32(layers.size()),
    };

    auto device = _image->get_device();

    set_image_layout(device, cmd_buffer, _image->get(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresource_range,
                                                        VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

    std::vector<VkBufferImageCopy> regions;

    if (to_ui32(layers.front().levels.size()) > 1) {

        auto offset = 0u;

        for (auto layer = 0u; layer < layers.size(); ++layer) {

            for (auto level = 0u; level < to_ui32(layers.front().levels.size()); ++level) {

                VkImageSubresourceLayers image_subresource
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = level,
                    .baseArrayLayer = layer,
                    .layerCount = 1,
                };

                VkExtent3D image_extent
                {
                    .width = layers[layer].levels[level].extent.x,
                    .height = layers[layer].levels[level].extent.y,
                    .depth = 1,
                };

                VkBufferImageCopy buffer_copy_region
                {
                    .bufferOffset = offset,
                    .imageSubresource = image_subresource,
                    .imageExtent = image_extent,
                };

                regions.push_back(buffer_copy_region);

                offset += layers[layer].levels[level].size;
            }
        }

    } else {

        VkImageSubresourceLayers subresource_layers
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = to_ui32(layers.size()),
        };

        auto size = _image->get_size();

        VkBufferImageCopy region
        {
            .bufferOffset = 0,
            .bufferRowLength = size.x,
            .bufferImageHeight = size.y,
            .imageSubresource = subresource_layers,
            .imageOffset = {},
            .imageExtent = { size.x, size.y, 1 },
        };

        regions.push_back(region);
    }

    device->call().vkCmdCopyBufferToImage(cmd_buffer, upload_buffer->get(), _image->get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                                                                to_ui32(regions.size()), regions.data());

    set_image_layout(device, cmd_buffer, _image->get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresource_range,
                                                        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

    return true;
}

bool staging::stage(VkCommandBuffer cmd_buf, index frame) {

    if (!staged.empty() && staged.count(frame) && !staged.at(frame).empty()) {

        for (auto& texture : staged.at(frame))
            texture->destroy_upload_buffer();

        staged.erase(frame);
    }

    if (todo.empty())
        return false;

    texture::list stage_done;

    for (auto& texture : todo) {

        if (!texture->stage(cmd_buf))
            continue;

        stage_done.push_back(texture);
    }

    if (!staged.count(frame))
        staged.emplace(frame, texture::list());

    for (auto& texture : stage_done) {

        if (!contains(staged.at(frame), texture))
            staged.at(frame).push_back(texture);

        remove(todo, texture);
    }

    return true;
}

} // lava

lava::texture::ptr lava::load_texture(device* device, file_format filename, texture_type type) {

    auto supported =    (filename.format == VK_FORMAT_R8G8B8A8_UNORM) ||
                        (device->get_features().textureCompressionBC && (filename.format == VK_FORMAT_BC3_UNORM_BLOCK)) ||
                        (device->get_features().textureCompressionASTC_LDR && (filename.format == VK_FORMAT_ASTC_8x8_UNORM_BLOCK)) ||
                        (device->get_features().textureCompressionETC2 && (filename.format == VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK));
    if (!supported)
        return nullptr;

    auto use_gli = has_extension(filename.path.c_str(), { "DDS", "KTX", "KMG" });
    auto use_stbi = false;

    if (!use_gli)
        use_stbi = has_extension(filename.path.c_str(), { "JPG", "PNG", "TGA", "BMP", "PSD", "GIF", "HDR", "PIC" });

    if (!use_gli && !use_stbi)
        return nullptr;

    file file(filename.path.c_str());
    scope_data temp_data(file.get_size(), false);

    if (file.is_open()) {

        if (!temp_data.allocate())
            return nullptr;

        if (is_file_error(file.read(temp_data.ptr)))
            return nullptr;
    }

    auto texture = texture::make();

    if (use_gli) {

        texture::layer::list layers;

        switch (type) {

            case texture_type::tex_2d: {

                gli::texture2d tex(file.is_open() ? gli::load(temp_data.ptr, temp_data.size) : gli::load(filename.path));
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
                if (!texture->create(device, size, filename.format, layers, type))
                    return nullptr;

                if (!texture->upload(tex.data(), tex.size()))
                    return nullptr;

                break;
            }

            case texture_type::array: {

                gli::texture2d_array tex(file.is_open() ? gli::load(temp_data.ptr, temp_data.size) : gli::load(filename.path));
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
                if (!texture->create(device, size, filename.format, layers, type))
                    return nullptr;

                if (!texture->upload(tex.data(), tex.size()))
                    return nullptr;

                break;
            }

            case texture_type::cube_map: {

                gli::texture_cube tex(file.is_open() ? gli::load(temp_data.ptr, temp_data.size) : gli::load(filename.path));
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
                if (!texture->create(device, size, filename.format, layers, type))
                    return nullptr;

                if (!texture->upload(tex.data(), tex.size()))
                    return nullptr;

                break;
            }
        }
    }
    else { // use_stbi

        i32 tex_width, tex_height, tex_channels = 0;
        stbi_uc* data = nullptr;

        if (file.is_open())
            data = stbi_load_from_memory((stbi_uc const*)temp_data.ptr, to_i32(temp_data.size), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
        else
            data = stbi_load(filename.path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

        if (!data)
            return nullptr;

        uv2 size = { tex_width, tex_height };
        if (!texture->create(device, size, VK_FORMAT_R8G8B8A8_UNORM))
            return nullptr;

        auto uploadSize = tex_width * tex_height * tex_channels * sizeof(char);
        auto result = texture->upload(data, uploadSize);

        stbi_image_free(data);

        if (!result)
            return nullptr;
    }

    return texture;
}

lava::texture::ptr lava::create_default_texture(device* device, uv2 size) {

    auto result = texture::make();

    if (!result->create(device, size, VK_FORMAT_R8G8B8A8_UNORM))
        return nullptr;

    bitmap_image image(size.x, size.y);
    checkered_pattern(64, 64, 255, 255, 255, image);

    image.bgr_to_rgb();

    sln::TypedLayout typed_layout((sln::PixelLength)size.x, (sln::PixelLength)size.y, (sln::Stride)(image.width() * image.bytes_per_pixel()));

    sln::ImageView<sln::PixelRGB_8u, sln::ImageModifiability::Mutable> img_rgb((uint8_t*)image.data(), typed_layout);

    sln::Image<sln::PixelRGBA_8u> const img_rgba = sln::convert_image<sln::PixelFormat::RGBA>(img_rgb, std::uint8_t{ 192 });

    if (!result->upload(img_rgba.data(), img_rgba.total_bytes()))
        return nullptr;

    return result;
}
