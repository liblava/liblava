/**
 * @file         liblava/resource/image.cpp
 * @brief        Vulkan image
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/resource/image.hpp"
#include "liblava/resource/format.hpp"
#include "liblava/util/log.hpp"

namespace lava {

//-----------------------------------------------------------------------------
image_data::~image_data() {
    if (data)
        free(data);
}

//-----------------------------------------------------------------------------
image::image(VkFormat format,
             VkImage vk_image)
: vk_image(vk_image) {
    info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {0, 0, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_SAMPLED_BIT
                 | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                 | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    subresource_range = {
        .aspectMask = format_aspect_mask(format),
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    view_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = vk_image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = {
            VK_COMPONENT_SWIZZLE_R,
            VK_COMPONENT_SWIZZLE_G,
            VK_COMPONENT_SWIZZLE_B,
            VK_COMPONENT_SWIZZLE_A},
        .subresourceRange = subresource_range,
    };
}

//-----------------------------------------------------------------------------
bool image::create(device::ptr d,
                   uv2 size,
                   VmaMemoryUsage memory_usage,
                   VmaAllocationCreateFlags allocation_flags) {
    device = d;

    info.extent = {size.x, size.y, 1};

    if (!vk_image) {
        VmaAllocationCreateInfo const create_info{
            .flags = allocation_flags,
            .usage = memory_usage,
        };

        if (failed(vmaCreateImage(device->alloc(),
                                  &info,
                                  &create_info,
                                  &vk_image,
                                  &allocation,
                                  nullptr))) {
            logger()->error("create image");
            return false;
        }
    }

    view_info.image = vk_image;
    view_info.subresourceRange = subresource_range;

    return device->vkCreateImageView(&view_info,
                                     &view);
}

//-----------------------------------------------------------------------------
void image::destroy(bool view_only) {
    if (view) {
        device->vkDestroyImageView(view);
        view = 0;
    }

    if (view_only)
        return;

    if (vk_image) {
        vmaDestroyImage(device->alloc(),
                        vk_image,
                        allocation);
        vk_image = 0;
        allocation = nullptr;
    }

    device = nullptr;
}

//-----------------------------------------------------------------------------
image::s_ptr create_image(device::ptr device,
                          VkFormat format,
                          uv2 size,
                          VkImage vk_image) {
    auto image = image::make(format, vk_image);

    if (!image->create(device, size, VMA_MEMORY_USAGE_AUTO))
        return nullptr;

    return image;
}

//-----------------------------------------------------------------------------
image::s_ptr grab_image(image::s_ptr source) {
    auto device = source->get_device();

    auto const size = source->get_size();
    auto const width = size.x;
    auto const height = size.y;

    auto image = image::make(VK_FORMAT_R8G8B8A8_UNORM);
    if (!image)
        return nullptr;

    image->set_tiling(VK_IMAGE_TILING_LINEAR);

    if (!image->create(device,
                       size,
                       VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
                       VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT))
        return nullptr;

    return one_time_submit(device, device->graphics_queue(), [&](VkCommandBuffer cmd_buf) {
        insert_image_memory_barrier(device,
                                    cmd_buf,
                                    image->get(),
                                    0,
                                    VK_ACCESS_TRANSFER_WRITE_BIT,
                                    VK_IMAGE_LAYOUT_UNDEFINED,
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                                    VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
        insert_image_memory_barrier(device,
                                    cmd_buf,
                                    source->get(),
                                    VK_ACCESS_MEMORY_READ_BIT,
                                    VK_ACCESS_TRANSFER_READ_BIT,
                                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                                    VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        if (support_blit(device->get_vk_physical_device(),
                         source->get_format())) {
            VkImageSubresourceLayers const subresource_layers = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .layerCount = 1,
            };

            VkImageBlit image_blit_region = {
                .srcSubresource = subresource_layers,
                .dstSubresource = subresource_layers,
            };

            VkOffset3D const blitSize = {
                .x = to_i32(width),
                .y = to_i32(height),
                .z = 1,
            };

            image_blit_region.srcOffsets[1] = blitSize;
            image_blit_region.dstOffsets[1] = blitSize;

            vkCmdBlitImage(
                cmd_buf,
                source->get(),
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image->get(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &image_blit_region,
                VK_FILTER_NEAREST);
        } else {
            VkImageSubresourceLayers const subresource_layers = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .layerCount = 1,
            };

            VkImageCopy const image_copy_region = {
                .srcSubresource = subresource_layers,
                .dstSubresource = subresource_layers,
                .extent = {width, height, 1},
            };

            vkCmdCopyImage(
                cmd_buf,
                source->get(),
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image->get(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &image_copy_region);
        }

        insert_image_memory_barrier(
            device,
            cmd_buf,
            image->get(),
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
        insert_image_memory_barrier(
            device,
            cmd_buf,
            source->get(),
            VK_ACCESS_TRANSFER_READ_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
    })
               ? image
               : nullptr;
}

} // namespace lava
