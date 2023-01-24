/**
 * @file         liblava/asset/write_image.cpp
 * @brief        Write image data to file
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/asset/write_image.hpp"
#include "liblava/resource/format.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace lava {

//-----------------------------------------------------------------------------
bool write_image_png(device_p device,
                     image::ptr image,
                     string_ref filename,
                     bool swizzle) {
    VkImageSubresource subResource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
    VkSubresourceLayout subResourceLayout;
    vkGetImageSubresourceLayout(device->get(),
                                image->get(),
                                &subResource,
                                &subResourceLayout);

    VmaAllocationInfo alloc_info{};
    vmaGetAllocationInfo(device->get_allocator()->get(),
                         image->get_allocation(),
                         &alloc_info);

    char const* img_data_ptr = nullptr;

    vkMapMemory(device->get(),
                alloc_info.deviceMemory,
                0,
                VK_WHOLE_SIZE,
                0,
                (void**) &img_data_ptr);

    img_data_ptr += subResourceLayout.offset;

    data img_data(as_ptr(img_data_ptr), subResourceLayout.size);
    auto const img_data_block_size = format_block_size(image->get_format());

    auto const size = image->get_size();
    auto const width = size.x;
    auto const height = size.y;

    unique_data rgb_data(height * width * img_data_block_size);
    auto const rgb_data_format = VK_FORMAT_R8G8B8_UNORM;
    auto const rgb_data_block_size = format_block_size(rgb_data_format);

    if (swizzle) {
        for (auto y = 0u; y < height; ++y) {
            auto const row_rgb = y * width * rgb_data_block_size;
            auto const row_img = y * subResourceLayout.rowPitch;
            for (auto x = 0u; x < width; ++x) {
                rgb_data.ptr[(x * rgb_data_block_size) + row_rgb] =
                    img_data.ptr[(x * img_data_block_size) + 2 + row_img];
                rgb_data.ptr[(x * rgb_data_block_size) + 1 + row_rgb] =
                    img_data.ptr[(x * img_data_block_size) + 1 + row_img];
                rgb_data.ptr[(x * rgb_data_block_size) + 2 + row_rgb] =
                    img_data.ptr[(x * img_data_block_size) + row_img];
            }
        }
    } else {
        for (auto y = 0u; y < height; ++y) {
            auto const row_rgb = y * width * rgb_data_block_size;
            auto const row_img = y * subResourceLayout.rowPitch;
            for (auto x = 0u; x < width; ++x) {
                rgb_data.ptr[(x * rgb_data_block_size) + row_rgb] =
                    img_data.ptr[(x * img_data_block_size) + row_img];
                rgb_data.ptr[(x * rgb_data_block_size) + 1 + row_rgb] =
                    img_data.ptr[(x * img_data_block_size) + 1 + row_img];
                rgb_data.ptr[(x * rgb_data_block_size) + 2 + row_rgb] =
                    img_data.ptr[(x * img_data_block_size) + 2 + row_img];
            }
        }
    }

    vkUnmapMemory(device->get(), alloc_info.deviceMemory);

    return stbi_write_png(str(filename),
                          width,
                          height,
                          rgb_data_block_size,
                          rgb_data.ptr,
                          width * rgb_data_block_size);
}

} // namespace lava
