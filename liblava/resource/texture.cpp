/**
 * @file         liblava/resource/texture.cpp
 * @brief        Vulkan texture
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/resource/texture.hpp"
#include "liblava/core/misc.hpp"
#include "liblava/resource/format.hpp"
#include "liblava/util/log.hpp"

namespace lava {

//-----------------------------------------------------------------------------
bool texture::create(device_p device,
                     uv2 size,
                     VkFormat format,
                     layer::list const& l,
                     texture_type t) {
    layers = l;
    type = t;

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

    VkSamplerCreateInfo sampler_info{
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
        log()->error("create texture sampler");
        return false;
    }

    img = image::make(format);

    if (type == texture_type::cube_map)
        img->set_flags(VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);

    auto view_type = VK_IMAGE_VIEW_TYPE_2D;
    if (type == texture_type::array)
        view_type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    else if (type == texture_type::cube_map)
        view_type = VK_IMAGE_VIEW_TYPE_CUBE;

    img->set_level_count(to_ui32(layers.front().levels.size()));
    img->set_layer_count(to_ui32(layers.size()));
    img->set_view_type(view_type);

    if (!img->create(device, size, VMA_MEMORY_USAGE_GPU_ONLY)) {
        log()->error("create texture image");
        return false;
    }

    descriptor.sampler = sampler;
    descriptor.imageView = img->get_view();
    descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    return true;
}

//-----------------------------------------------------------------------------
void texture::destroy() {
    destroy_upload_buffer();

    if (sampler) {
        if (img)
            if (auto device = img->get_device())
                device->vkDestroySampler(sampler);

        sampler = VK_NULL_HANDLE;
    }

    if (img) {
        img->destroy();
        img = nullptr;
    }
}

//-----------------------------------------------------------------------------
void texture::destroy_upload_buffer() {
    upload_buffer = nullptr;
}

//-----------------------------------------------------------------------------
bool texture::upload(void const* data,
                     size_t data_size) {
    upload_buffer = buffer::make();

    return upload_buffer->create(img->get_device(),
                                 data,
                                 data_size,
                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 false,
                                 VMA_MEMORY_USAGE_CPU_TO_GPU);
}

//-----------------------------------------------------------------------------
bool texture::stage(VkCommandBuffer cmd_buf) {
    if (!upload_buffer || !upload_buffer->valid()) {
        log()->error("stage texture");
        return false;
    }

    VkImageSubresourceRange subresource_range{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = to_ui32(layers.front().levels.size()),
        .baseArrayLayer = 0,
        .layerCount = to_ui32(layers.size()),
    };

    auto device = img->get_device();

    set_image_layout(device, cmd_buf, img->get(), VK_IMAGE_LAYOUT_UNDEFINED,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresource_range,
                     VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

    std::vector<VkBufferImageCopy> regions;

    if (to_ui32(layers.front().levels.size()) > 1) {
        auto offset = 0u;

        for (auto layer = 0u; layer < layers.size(); ++layer) {
            for (auto level = 0u;
                 level < to_ui32(layers.front().levels.size());
                 ++level) {
                VkImageSubresourceLayers image_subresource{
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = level,
                    .baseArrayLayer = layer,
                    .layerCount = 1,
                };

                VkExtent3D image_extent{
                    .width = layers[layer].levels[level].extent.x,
                    .height = layers[layer].levels[level].extent.y,
                    .depth = 1,
                };

                VkBufferImageCopy buffer_copy_region{
                    .bufferOffset = offset,
                    .imageSubresource = image_subresource,
                    .imageExtent = image_extent,
                };

                regions.push_back(buffer_copy_region);

                offset += layers[layer].levels[level].size;
            }
        }
    } else {
        VkImageSubresourceLayers subresource_layers{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = to_ui32(layers.size()),
        };

        auto size = img->get_size();

        VkBufferImageCopy region{
            .bufferOffset = 0,
            .bufferRowLength = size.x,
            .bufferImageHeight = size.y,
            .imageSubresource = subresource_layers,
            .imageOffset = {},
            .imageExtent = { size.x, size.y, 1 },
        };

        regions.push_back(region);
    }

    device->call().vkCmdCopyBufferToImage(cmd_buf,
                                          upload_buffer->get(),
                                          img->get(),
                                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                          to_ui32(regions.size()),
                                          regions.data());

    set_image_layout(device,
                     cmd_buf,
                     img->get(),
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                     subresource_range,
                     VK_PIPELINE_STAGE_TRANSFER_BIT,
                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

    return true;
}

//-----------------------------------------------------------------------------
bool staging::stage(VkCommandBuffer cmd_buf,
                    index frame) {
    if (!staged.empty() && staged.count(frame)
        && !staged.at(frame).empty()) {
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

} // namespace lava
