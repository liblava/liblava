// file      : liblava/resource/image.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/resource/image.hpp>

namespace lava {

image::image(VkFormat format, VkImage vk_image) : _id(ids::next()), vk_image(vk_image) {

    info =
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = { 0, 0, 1 },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    subresource_range =
    {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    view_info =
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = vk_image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A },
        .subresourceRange = subresource_range,
    };
}

image::~image() { ids::free(_id); }

image::ptr image::make(VkFormat format, VkImage vk_image) { return std::make_shared<image>(format, vk_image);  }

image::ptr image::make(VkFormat format, device* device, uv2 size, VkImage vk_image) {

    auto result = make(format, vk_image);

    if (!result->create(device, size))
        return nullptr;

    return result;
}

bool image::create(device* device, uv2 size, VmaMemoryUsage memory_usage, bool mip_levels_generation) {

    dev = device;

    info.extent = { size.x, size.y, 1 };

    if (!vk_image) {

        VmaAllocationCreateInfo allocInfo
        {
            .usage = memory_usage,
        };

        if (failed(vmaCreateImage(dev->alloc(), &info, &allocInfo, &vk_image, &allocation, nullptr))) {

            log()->error("image::create vmaCreateImage failed");
            return false;
        }
    }

    view_info.image = vk_image;
    view_info.subresourceRange = subresource_range;

    return device->vkCreateImageView(&view_info, memory::alloc(), &view);
}

void image::destroy(bool only_view) {

    if (view) {

        dev->vkDestroyImageView(view, memory::alloc());
        view = nullptr;
    }

    if (only_view)
        return;

    if (vk_image) {

        vmaDestroyImage(dev->alloc(), vk_image, allocation);
        vk_image = nullptr;
        allocation = nullptr;
    }

    dev = nullptr;
}

} // lava
